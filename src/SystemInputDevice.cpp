#include "stdafx.h"

#include "SystemInputDevice.h"
#include "raw_input.h"
#include "thread_control_interface.h"
#include "Log.h"

SystemInputDevice::SystemInputDevice(InputMode i_inputMode, utils::IMessageQueue& i_inputQueue)
    : IInputDevice(i_inputQueue)
    , m_inputMode(i_inputMode)
    , m_inputVar()
    , m_inputThread({ .thread_name = "input thread", .thread_prologue = {&SystemInputDevice::FirstRun, this} })
{
    utils::async(m_inputThread, &SystemInputDevice::OnRun, this);
}

std::string SystemInputDevice::GetTextReceived(utils::IYielder* i_yielder) const
{
    m_isInputRequested = true;
    while (!m_inputRequest.has_value())
    {
        if (i_yielder)
        {
            i_yielder->DoYieldWithResult(utils::IYielder::Mode::Forced).ignoreResult();
        }
        else
        {
            std::unique_lock lk(m_mutex);
            m_cv.wait(lk, [this]() {return m_inputRequest.has_value(); });
        }
    }
    m_isInputRequested = false;
    std::string returnRequest = std::move(*m_inputRequest);
    m_inputRequest.reset();
    return returnRequest;
}

void SystemInputDevice::OnRun()
{
    utils::async_waitable<void> waitable = utils::async(m_inputThread, &SystemInputDevice::OnRun, this);
    if (int c = GetChar(); c != EOF && c != '\n')
    {
        m_inputVar += c;
        if (m_inputMode == InputMode::Line)
        {
            return;
        }
    }
    if (m_isInputRequested)
    {
        m_inputRequest = m_inputVar;
        m_cv.notify_one();
    }
    if (m_inputVar.find(m_exitStrToken) != m_inputVar.npos)
    {
        utils::secs elapsedSeconds = std::chrono::duration_cast<utils::secs>(utils::timepoint::clock::now() - m_startTime);
        WARNING_LOG("On Exiting", "Exiting.... with elapsed time: {}s", elapsedSeconds.count());
        OnCancel();
        waitable.Cancel();
    }
    else
    {
        utils::async_waitable<bool> processWaitable = utils::Access<AccessKey>(sig_onInput).EmitAsync(m_inputVar);
        if (auto processResult = processWaitable.GetResult(); processResult.isOk() && !processResult.unwrap())
        {
            waitable.Cancel();
        }
    }
    m_inputVar.clear();
}

void SystemInputDevice::OnCancel()
{
    utils::Access<IInputDevice::AccessKey>(sig_onQuit).EmitAsync();
}

void SystemInputDevice::FirstRun()
{
    m_startTime = utils::timepoint::clock::now();
}

int SystemInputDevice::GetChar()
{
    return m_inputMode == InputMode::Char ? utils::raw_getch() : std::cin.get();
}
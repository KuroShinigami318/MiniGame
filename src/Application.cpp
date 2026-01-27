#include "stdafx.h"
#include "Application.h"
#include "IHeartBeats.h"
#include "IInputDevice.h"
#include "Game.h"
#include "Log.h"

void ClearRender()
{
	ScopedVirtualConsoleMode();
    std::cout << "\033c\033[?25l";
}

Application::Application(utils::unique_ref<utils::IHeartBeats> i_heart)
    : m_isExiting(false), m_exitReason(ExitReason::_COUNT)
    , m_actualElapsed(0)
    , m_heart(std::move(i_heart))
    , m_frameThread({ &Application::FramePrologue, this }, { &Application::FrameEpilogue, this })
{
    utils::async(GetNextFrameMessageQueue(), &Application::CreateGame, this);
    utils::Log::RegisterWriter<utils::Log::DefaultConsoleWriter>();
}

Application::~Application() = default;

utils::MessageSink_mt& Application::GetNextFrameMessageQueue()
{
    return m_frameThread.GetNextFrameMessageQueue();
}

utils::MessageSink& Application::GetThisFrameMessageQueue()
{
    return m_frameThread.GetFrameMessageQueue();
}

utils::IRecursiveControl& Application::GetRecursiveControl()
{
    return m_frameThread;
}

void Application::Execute()
{
    utils::steady_clock::time_point beginFrameTimePoint = utils::steady_clock::now();
    utils::steady_clock::time_point lastFrameTimePoint = utils::steady_clock::now();
    START:
    while (!m_isExiting)
    {
        m_actualElapsed = utils::steady_clock::now() - beginFrameTimePoint;
        beginFrameTimePoint = utils::steady_clock::now();
        m_frameThread.Submit(GetElapsedSeconds());
        m_mainQueue.dispatch();
        {
            Render();
        }
        m_frameResult = m_frameThread.Wait();
        m_actualElapsed = utils::steady_clock::now() - beginFrameTimePoint;
        auto remaining = m_heart->cast_to_duration() - m_actualElapsed;
        lastFrameTimePoint += utils::IsInBounds(remaining.count(), 0., std::numeric_limits<double>::infinity()) ? m_actualElapsed + std::chrono::duration_cast<utils::nanosecs>(remaining) : m_actualElapsed;
        std::unique_lock lk(m_mutex);
        m_cv.wait_until(lk, lastFrameTimePoint, [this]() {return m_isExiting; });
    }
    if (m_optExitCallback.has_value())
    {
        utils::async(GetNextFrameMessageQueue(), *m_optExitCallback);
    }
    if (m_exitReason == ExitReason::Reload)
    {
        RequestExit(ExitReason::_COUNT);
        goto START;
    }
    utils::async(GetNextFrameMessageQueue(), &Application::DestroyGame, this);
    m_frameThread.Submit(GetElapsedSeconds());
    m_frameThread.Wait();
}

void Application::RequestExit(ExitReason i_reason)
{
    m_isExiting = i_reason != ExitReason::_COUNT;
    m_exitReason = i_reason;
    m_optExitCallback.reset();
    switch (i_reason)
    {
    case ExitReason::Reload:
    {
        m_optExitCallback.emplace(&Game::OnReload, m_game.get());
    }
    break;
    case ExitReason::Exit:
    {
        m_optExitCallback.emplace(&Game::OnExit, m_game.get());
    }
    break;
    }
}

const bool Application::IsExiting() const
{
    return m_isExiting;
}

float Application::GetElapsedSeconds() const
{
    return std::chrono::duration_cast<utils::duration<float>>(m_actualElapsed).count();
}

void Application::RegisterInputDevice(IInputDevice& i_inputDevice)
{
    if (!m_game)
    {
        utils::async(m_mainQueue, &Application::RegisterInputDevice, this, i_inputDevice);
    }
    m_connections.push_back(i_inputDevice.sig_onQuit.Connect(
        [this]()
        {
            RequestExit(ExitReason::Exit);
        }));
    m_connections.push_back(i_inputDevice.sig_onInput.Connect(
        [this](std::string input)
        {
            if (input.find("r") != std::string::npos)
            {
                RequestExit(ExitReason::Reload);
            }
            return true;
        }));
    if (m_game)
    {
        utils::async(GetNextFrameMessageQueue(), &Game::RegisterInputDevice, m_game.get(), i_inputDevice);
    }
}
void Application::UnregisterInputDevice(IInputDevice& i_inputDevice)
{
    m_connections.clear();
    if (m_game)
    {
        utils::async(GetNextFrameMessageQueue(), &Game::UnregisterInputDevice, m_game.get(), i_inputDevice);
        m_frameThread.Submit(GetElapsedSeconds());
		m_frameThread.Wait();
    }
}

void Application::FramePrologue(float i_elapsed)
{
    if (m_game)
    {
        m_game->FramePrologue(i_elapsed);
    }
}

Application::FrameResult Application::FrameEpilogue() const
{
    FrameResult frameResult{.renderStream = std::make_unique<std::stringstream>()};
    if (m_game)
    {
        m_game->FrameEpilogue(*frameResult.renderStream);
    }
    return frameResult;
}

void Application::CreateGame()
{
    m_game = std::make_unique<Game>(GetNextFrameMessageQueue(), GetThisFrameMessageQueue(), GetRecursiveControl());
}

void Application::DestroyGame()
{
    m_game.reset();
}

void Application::Render()
{
    if (!m_frameResult.renderStream)
    {
        return;
    }
    if (!m_previousFrameResult.renderStream || m_previousFrameResult.renderStream->str() != m_frameResult.renderStream->str())
    {
        ClearRender();
        std::cout << m_frameResult.renderStream->rdbuf() << std::flush << std::endl;
        m_previousFrameResult = std::move(m_frameResult);
    }
}
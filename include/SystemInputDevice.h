#pragma once
#include "IInputDevice.h"

class SystemInputDevice : public IInputDevice
{
public:
    DeclareScopedEnum(InputMode, uint8_t, Char, Line)

public:
    SystemInputDevice(InputMode i_inputMode, utils::IMessageQueue& i_inputQueue);
    std::string GetTextReceived(utils::IYielder* i_yielder = nullptr) const override;

private:
    void OnRun();
    void OnCancel();
    void FirstRun();
    int GetChar();

    InputMode m_inputMode;
    const std::string m_exitStrToken{ "q" };
    std::string m_inputVar;
    mutable std::optional<std::string> m_inputRequest;
    mutable bool m_isInputRequested = false;
    utils::timepoint m_startTime;
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cv;
    utils::message_thread m_inputThread;
};
DefineScopeEnumOperatorImpl(InputMode, SystemInputDevice)
#pragma once

namespace utils
{
class IYielder;
}

class IInputDevice
{
protected:
    struct AccessKey;
    IInputDevice(utils::IMessageQueue& i_inputQueue)
        : sig_onInput(&i_inputQueue)
        , sig_onQuit(&i_inputQueue)
    {
    }

public:
    virtual ~IInputDevice() = default;
    virtual std::string GetTextReceived(utils::IYielder* i_yielder = nullptr) const = 0;
    utils::Signal_mt<bool(std::string), AccessKey> sig_onInput;
    utils::Signal_mt<void(), AccessKey> sig_onQuit;
};
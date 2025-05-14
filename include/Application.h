#pragma once
#include "FrameThread.h"

class IInputDevice;

class Application
{
private:
    struct FrameResult
    {
        std::unique_ptr<std::stringstream> renderStream;
    };
    struct SignalKey;

public:
    DeclareScopedEnum(ExitReason, uint8_t, Reload, Exit);
    using MessageType = utils::CallableBound<void()>;

    Application() = delete;
    Application(utils::unique_ref<utils::IHeartBeats> i_heart);
    ~Application();

    utils::MessageSink_mt& GetNextFrameMessageQueue();
    utils::MessageSink& GetThisFrameMessageQueue();
    utils::IRecursiveControl& GetRecursiveControl();

    void Execute();
    void RequestExit(ExitReason i_reason);
    const bool IsExiting() const;
    void RegisterInputDevice(IInputDevice& i_inputDevice);
    void UnregisterInputDevice(IInputDevice& i_inputDevice);

private:
    float GetElapsedSeconds() const;
    void FramePrologue(float i_elapsed);
    FrameResult FrameEpilogue() const;
    void CreateGame();
    void DestroyGame();
    void Render();

    bool m_isExiting;
    ExitReason m_exitReason;
    std::optional<MessageType> m_optExitCallback;
    utils::MessageSink_mt m_mainQueue;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    utils::nanosecs m_actualElapsed;
    FrameResult m_previousFrameResult;
    FrameResult m_frameResult;
    utils::unique_ref<utils::IHeartBeats> m_heart;
    utils::FrameThread<FrameResult(float)> m_frameThread;
    std::vector<utils::Connection> m_connections;
    std::unique_ptr<class Game> m_game;
};
DefineScopeEnumOperatorImpl(ExitReason, Application);
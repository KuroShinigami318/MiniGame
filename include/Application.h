#pragma once
#include "FrameThread.h"
#include "ExitReason.h"
#include <string>
#include <vector>

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
	void SyncWithFrameThread();

    bool m_isExiting;
    float m_multiplier;
    ExitReason m_exitReason;
    std::optional<MessageType> m_optExitCallback;
    utils::MessageSink_mt m_mainQueue;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    utils::nanosecs m_actualElapsed;
    FrameResult m_previousFrameResult;
    FrameResult m_frameResult;
    std::vector<std::string> m_previousRows;
    int m_renderWidth = 0;
    int m_renderHeight = 0;
    bool m_firstRender = true;
    utils::unique_ref<utils::IHeartBeats> m_heart;
    utils::FrameThread<FrameResult(float)> m_frameThread;
    std::vector<utils::Connection> m_connections;
    std::unique_ptr<class Game> m_game;
};
#pragma once
#include "RendererType.h"

namespace utils
{
struct SystemClock;
}

class IInputDevice;

class Game
{
public:
	Game(utils::MessageSink_mt& i_nextFrameQueue, utils::MessageSink& i_thisFrameQueue, utils::IRecursiveControl& i_recursiveControl);
	~Game();
	void RegisterInputDevice(IInputDevice& i_inputDevice);
	void UnregisterInputDevice(IInputDevice& i_inputDevice);
	void FramePrologue(float i_elapsed);
	void FrameEpilogue(RendererT& o_renderStream) const;
	void OnReload();
	void OnExit();

private:
	utils::MessageSink_mt& m_nextFrameQueue;
	utils::MessageSink& m_thisFrameQueue;
   utils::IRecursiveControl& m_recursiveControl;
	utils::unique_ref<utils::SystemClock> m_systemClock;
	utils::unique_ref<class GameControl> m_gameControl;
	utils::unique_ref<class UIManager> m_uiManager;
	utils::unique_ref<class LevelSystem> m_levelSystem;
	utils::unique_ref<class MapSystem> m_mapSystem;
	std::vector<utils::Connection> m_connections;
};
#pragma once

namespace utils
{
class AsyncScopedHelper;
struct SystemClock;
}

class IGameControl;
class IUIManager;
class IMap;
class ILevel;

class MapSystem
{
public:
	MapSystem(IGameControl& i_gameControl, IUIManager& i_uiManager, const utils::SystemClock& i_systemClock, utils::IMessageQueue& i_thisFrameQueue);
	~MapSystem();
	void SetLevel(std::unique_ptr<ILevel> i_level);

private:
	void OnMapChanged(std::unique_ptr<IMap> i_map);
	void OnFinishedLevel();
	void Update(float i_elapsed);

private:
	IGameControl& m_gameControl;
	IUIManager& m_uiManager;
	utils::IMessageQueue& m_thisFrameQueue;
	std::unique_ptr<IMap> m_map;
	std::unique_ptr<class ScoresComponent> m_scoresComponent;
	utils::Connection m_updateConnection;
	utils::Connection m_onFinishedLevelConnection;
	utils::unique_ref<utils::AsyncScopedHelper> m_asyncScopedHelper;
};
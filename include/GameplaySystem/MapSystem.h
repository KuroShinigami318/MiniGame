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
class GameplayWindow;

class MapSystem
{
public:
	MapSystem(IGameControl& i_gameControl, IUIManager& i_uiManager);
	~MapSystem();
	void SetLevel(std::unique_ptr<ILevel> i_level);

private:
	void OnMapChanged(std::unique_ptr<IMap> i_map);
	void OnFinishedLevel();
	void Update(float i_elapsed);

private:
	IGameControl& m_gameControl;
	IUIManager& m_uiManager;
	GameplayWindow* m_gameplayWindow;
	utils::Connection m_updateConnection;
	utils::Connection m_onFinishedLevelConnection;
	utils::unique_ref<utils::AsyncScopedHelper> m_asyncScopedHelper;
};
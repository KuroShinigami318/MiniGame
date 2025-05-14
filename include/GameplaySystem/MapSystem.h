#pragma once

namespace utils
{
struct SystemClock;
}

class IGameControl;
class IUIManager;
class IMap;
class ILevel;

class MapSystem
{
public:
	MapSystem(IGameControl& i_gameControl, IUIManager& i_uiManager, const utils::SystemClock& i_systemClock);
	void SetLevel(std::unique_ptr<ILevel> i_level);

private:
	void OnMapChanged(std::unique_ptr<IMap> i_map);
	void OnFinishedLevel();
	void Update(float i_elapsed);

private:
	IGameControl& m_gameControl;
	IUIManager& m_uiManager;
	std::unique_ptr<IMap> m_map;
	utils::Connection m_updateConnection;
	utils::Connection m_onFinishedLevelConnection;
};
#pragma once
#include "ILevel.h"
#include "random_generator.h"

namespace utils
{
class AsyncScopedHelper;
struct SystemClock;
class TimerDelayer;
}
class IGameControl;
class IUIManager;

class LevelSystem
{
private:
	struct SignalKey;

public:
	LevelSystem(const IGameControl& i_gameControl, const IUIManager& i_uiManager);
	~LevelSystem();
	void StartLevelGeneration();

private:
	void Update(float i_elapsed);
	std::unique_ptr<ILevel> GenerateRandomLevel();
	std::unique_ptr<ILevel> GenerateRandomLevel(size_t i_width, size_t i_height);
	int ClampRandomGeneratedValue(int i_value, int i_min, int i_max) const;
	void OnPlayerBroken(size_t i_width, size_t i_height, ILevel& o_level);
	void RespawnPlayer(size_t i_width, size_t i_height, ILevel& o_level);
	void StartLevelGenerationAsync();

public:
	utils::Signal_public<void(std::unique_ptr<ILevel>), SignalKey> sig_onLevelChanged;

private:
	const IGameControl& m_gameControl;
	const IUIManager& m_uiManager;
	utils::unique_ref<utils::TimerDelayer> m_levelGenerationDelayer;
	utils::Connection m_gameControlConnection;
	utils::Connection m_updateConnection;
	utils::Connection m_playerConnection;
	utils::Connection m_levelFinishedConnection;
	utils::Connection m_delayerExpiredConnection;
	utils::RandomGenerator<int, 5, 60> m_randomGenerator;
	utils::unique_ref<utils::AsyncScopedHelper> m_asyncScopedHelper;
};
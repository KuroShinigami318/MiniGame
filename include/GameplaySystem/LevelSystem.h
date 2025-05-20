#pragma once
#include "ILevel.h"
#include "random_generator.h"

namespace utils
{
struct SystemClock;
}
class IGameControl;

class LevelSystem
{
private:
	struct SignalKey;

public:
	LevelSystem(const IGameControl& i_gameControl, const utils::SystemClock& i_systemClock, utils::IMessageQueue& i_thisFrameQueue);

private:
	void Update(float i_elapsed);
	std::unique_ptr<ILevel> GenerateRandomLevel(size_t i_width, size_t i_height);
	int ClampRandomGeneratedValue(int i_value, int i_min, int i_max) const;

public:
	utils::Signal_public<void(std::unique_ptr<ILevel>), SignalKey> sig_onLevelChanged;

private:
	utils::IMessageQueue& m_thisFrameQueue;
	const IGameControl& m_gameControl;
	const utils::SystemClock& m_systemClock;
	utils::Connection m_updateConnection;
	utils::Connection m_levelFinishedConnection;
	utils::RandomGenerator<int, 5, 60> m_randomGenerator;
};
#include "stdafx.h"
#include "GameplaySystem/LevelSystem.h"
#include "GameplaySystem/Level.h"
#include "Components/PlayerComponent.h"
#include "Components/ItemComponent.h"
#include "UI/Map.h"
#include "Log.h"
#include "system_clock.h"
#include <unordered_set>

namespace
{
constexpr const int k_heightLimit = 10;
}

LevelSystem::LevelSystem(const IGameControl& i_gameControl, const utils::SystemClock& i_systemClock, utils::IMessageQueue& i_thisFrameQueue)
	: m_gameControl(i_gameControl)
	, m_systemClock(i_systemClock)
	, m_thisFrameQueue(i_thisFrameQueue)
{
	m_updateConnection = i_systemClock.sig_onTick.Connect(&LevelSystem::Update, this);
	utils::async(m_thisFrameQueue,
		[this]()
		{
			utils::Access<SignalKey>(sig_onLevelChanged).Emit(GenerateRandomLevel(m_randomGenerator(), ClampRandomGeneratedValue(m_randomGenerator(), (m_randomGenerator.min)(), k_heightLimit)));
		});
}

int LevelSystem::ClampRandomGeneratedValue(int i_value, int i_min, int i_max) const
{
	return (i_value % (i_max - i_min)) + (i_min);
}

void LevelSystem::Update(float i_elapsed)
{
	
}

std::unique_ptr<ILevel> LevelSystem::GenerateRandomLevel(size_t i_width, size_t i_height)
{
	std::unordered_set<Position> positions;
	utils::unique_ref<Map> map = utils::make_unique<Map>();
	map->Initialize(i_width, i_height);
	IMap::MapHolder mapHolder{ std::make_unique<PlayerComponent>(k_defaultVeclocity, m_gameControl, m_systemClock) };
	Position initialPosition{m_randomGenerator() % static_cast<int>(i_width), m_randomGenerator() % static_cast<int>(i_height)};
	positions.insert(initialPosition);
	mapHolder.connection = dynamic_cast<PlayerComponent&>(*mapHolder.component).sig_onMoved.Connect(&Map::OnComponentMoved, map.get(), initialPosition);
	map->AddComponent(std::move(mapHolder), initialPosition);

	size_t generateSize = (size_t)m_randomGenerator() % ((i_width * i_height) - 1);
	for (size_t i = 0; i < generateSize; ++i)
	{
		Position position{ m_randomGenerator() % static_cast<int>(i_width), m_randomGenerator() % static_cast<int>(i_height) };
		while (positions.contains(position))
		{
			position.x = m_randomGenerator() % static_cast<int>(i_width);
			position.y = m_randomGenerator() % static_cast<int>(i_height);
		}
		positions.insert(position);
		map->AddComponent(IMap::MapHolder(std::make_unique<ItemComponent>()), position);
	}

	std::unique_ptr<Level> level(new Level(std::move(map)));
	m_levelFinishedConnection = level->sig_onFinishedLevel.Connect(
		[this]()
		{
			utils::Access<SignalKey>(sig_onLevelChanged).Emit(GenerateRandomLevel(m_randomGenerator(), m_randomGenerator() % k_heightLimit));
		});
	return level;
}
#include "stdafx.h"
#include "GameplaySystem/LevelSystem.h"
#include "AsyncScopedHelper.h"
#include "GameplaySystem/Level.h"
#include "Components/PlayerComponent.h"
#include "Components/ItemComponent.h"
#include "Components/TrapComponent.h"
#include "Components/WallComponent.h"
#include "UI/Map.h"
#include "Log.h"
#include "system_clock.h"
#include <unordered_set>

namespace
{
constexpr const int k_heightLimit = 10;
template <typename T, typename RandomGenerator>
size_t GenerateMapComponents(RandomGenerator& i_randomGenerator, Map& o_map, std::unordered_set<Position>& o_positions, const UIContext& i_uiContext, ILevel& i_level, size_t i_width, size_t i_height, size_t& o_remainingPossibleComponents)
{
	size_t generateSize = (size_t)i_randomGenerator() % (o_remainingPossibleComponents + 1);
	for (size_t i = 0; i < generateSize; ++i)
	{
		Position position{ i_randomGenerator() % static_cast<int>(i_width), i_randomGenerator() % static_cast<int>(i_height) };
		while (o_positions.contains(position))
		{
			position.x = i_randomGenerator() % static_cast<int>(i_width);
			position.y = i_randomGenerator() % static_cast<int>(i_height);
		}
		o_positions.insert(position);
		o_map.AddComponent(IMap::MapHolder(std::make_unique<T>(i_uiContext, i_level)), position);
	}
	o_remainingPossibleComponents -= generateSize;
	return generateSize;
}
}

LevelSystem::LevelSystem(const IGameControl& i_gameControl, const utils::SystemClock& i_systemClock, utils::IMessageQueue& i_thisFrameQueue, utils::IMessageQueue& i_nextFrameQueue)
	: m_gameControl(i_gameControl)
	, m_systemClock(i_systemClock)
	, m_thisFrameQueue(i_thisFrameQueue)
	, m_nextFrameQueue(i_nextFrameQueue)
	, m_asyncScopedHelper(utils::make_unique<utils::AsyncScopedHelper>())
{
	m_updateConnection = i_systemClock.sig_onTick.Connect(&LevelSystem::Update, this);
	m_asyncScopedHelper->StartOptionalTask(m_thisFrameQueue,
		[this]()
		{
			utils::Access<SignalKey>(sig_onLevelChanged).Emit(GenerateRandomLevel());
		});
}

LevelSystem::~LevelSystem() = default;

int LevelSystem::ClampRandomGeneratedValue(int i_value, int i_min, int i_max) const
{
	return (i_value % (i_max - i_min)) + (i_min);
}

void LevelSystem::Update(float i_elapsed)
{
	
}

std::unique_ptr<ILevel> LevelSystem::GenerateRandomLevel()
{
	return GenerateRandomLevel(m_randomGenerator(), ClampRandomGeneratedValue(m_randomGenerator(), (m_randomGenerator.min)(), k_heightLimit));
}

std::unique_ptr<ILevel> LevelSystem::GenerateRandomLevel(size_t i_width, size_t i_height)
{
	UIContext uiContext{ m_thisFrameQueue, m_nextFrameQueue };
	std::unordered_set<Position> positions;
	utils::unique_ref<Map> map = utils::make_unique<Map>(uiContext);
	Map& mapRef = *map;
	std::unique_ptr<Level> level(new Level(std::move(map)));
	mapRef.Initialize(i_width, i_height);
	IMap::MapHolder mapHolder{ std::make_unique<PlayerComponent>(uiContext, k_defaultVeclocity, m_gameControl, m_systemClock) };
	Position initialPosition{m_randomGenerator() % static_cast<int>(i_width), m_randomGenerator() % static_cast<int>(i_height)};
	positions.insert(initialPosition);
	PlayerComponent& playerComponent = dynamic_cast<PlayerComponent&>(*mapHolder.component);
	mapHolder.connections.insert_or_assign(utils::get_type_index(playerComponent.sig_onMoved), playerComponent.sig_onMoved.Connect(&Map::OnComponentMoved, &mapRef, initialPosition));
	m_playerConnection = playerComponent.sig_onBroken.Connect(&LevelSystem::OnPlayerBroken, this, i_width, i_height, *level);
	mapRef.AddComponent(std::move(mapHolder), initialPosition);

	size_t remainingPossibleComponents = (i_width * i_height) - 1; // -1 for the player component
	size_t generatedItems = GenerateMapComponents<ItemComponent>(m_randomGenerator, mapRef, positions, uiContext, *level, i_width, i_height, remainingPossibleComponents);
	size_t generatedWalls = GenerateMapComponents<WallComponent>(m_randomGenerator, mapRef, positions, uiContext, *level, i_width, i_height, remainingPossibleComponents);
	size_t generatedTraps = GenerateMapComponents<TrapComponent>(m_randomGenerator, mapRef, positions, uiContext, *level, i_width, i_height, remainingPossibleComponents);

	level->SetObjectiveScore(generatedItems);
	level->SetAllowedRespawns(generatedTraps - 1);
	level->CheckValidMap().assertSuccess();
	m_levelFinishedConnection = level->sig_onFinishedLevel.Connect(
		[this]()
		{
			m_asyncScopedHelper->StartOptionalTask(m_thisFrameQueue, [this]()
			{
				utils::Access<SignalKey>(sig_onLevelChanged).Emit(GenerateRandomLevel());
			});
		});
	m_gameControlConnection = m_gameControl.sig_onControlReceived.Connect(
		[](ILevel& level, IGameControl::ControlType i_controlType)
		{
			if (i_controlType == IGameControl::ControlType::ToggleDebug)
			{
				level.SetDebugEnable(!level.IsDebugEnabled());
			}
		}, *level);
	return level;
}

void LevelSystem::OnPlayerBroken(size_t i_width, size_t i_height, ILevel& o_level)
{
	if (!o_level.Respawn())
	{
		o_level.ResetScore();
		o_level.DecreaseScore();
	}
	else
	{
		RespawnPlayer(i_width, i_height, o_level);
	}
}

void LevelSystem::RespawnPlayer(size_t i_width, size_t i_height, ILevel& o_level)
{
	Level& level = dynamic_cast<Level&>(o_level);
	UIContext uiContext{ m_thisFrameQueue, m_nextFrameQueue };
	IMap::MapHolder mapHolder{ std::make_unique<PlayerComponent>(uiContext, k_defaultVeclocity, m_gameControl, m_systemClock) };
	Position playerPosition{ m_randomGenerator() % static_cast<int>(i_width), m_randomGenerator() % static_cast<int>(i_height) };
	while (level.RetrieveComponent(playerPosition))
	{
		playerPosition.x = m_randomGenerator() % static_cast<int>(i_width);
		playerPosition.y = m_randomGenerator() % static_cast<int>(i_height);
	}
	PlayerComponent& playerComponent = dynamic_cast<PlayerComponent&>(*mapHolder.component);
	mapHolder.connections.insert_or_assign(utils::get_type_index(playerComponent.sig_onMoved), playerComponent.sig_onMoved.Connect(&Map::OnComponentMoved, dynamic_cast<Map*>(&level.GetMap()), playerPosition));
	m_playerConnection = playerComponent.sig_onBroken.Connect(&LevelSystem::OnPlayerBroken, this, i_width, i_height, o_level);
	level.AddComponent(std::move(mapHolder), playerPosition);
}
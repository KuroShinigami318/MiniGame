#include "stdafx.h"
#include "GameplaySystem/LevelSystem.h"
#include "AsyncScopedHelper.h"
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
	UIContext uiContext{ m_thisFrameQueue };
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
	mapRef.AddComponent(std::move(mapHolder), initialPosition);

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
		mapRef.AddComponent(IMap::MapHolder(std::make_unique<ItemComponent>(uiContext, *level)), position);
	}

	level->SetObjectiveScore(generateSize);
	m_levelFinishedConnection = level->sig_onFinishedLevel.Connect(
		[this]()
		{
			m_asyncScopedHelper->StartOptionalTask(m_thisFrameQueue, [this]()
			{
				utils::Access<SignalKey>(sig_onLevelChanged).Emit(GenerateRandomLevel());
			});
		});
	return level;
}
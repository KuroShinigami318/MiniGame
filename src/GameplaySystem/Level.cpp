#include "stdafx.h"
#include "GameplaySystem/Level.h"

Level::Level(utils::unique_ref<IMap> i_map)
	: IUIComponent(i_map->GetUIContext())
	, m_map(std::move(i_map))
{

}

void Level::Render(RendererT& o_renderStream) const
{
	m_map->Render(o_renderStream);
}

utils::unique_ref<IComponent> Level::Clone()
{
	return utils::make_unique<Level>(utils::dynamic_unique_cast<IMap>(m_map->Clone()));
}

IComponent* Level::RetrieveComponent(Position& io_position)
{
	return m_map->RetrieveComponent(io_position);
}

IMap::MapHolder* Level::RetrieveMapHolder(Position& io_position)
{
	return m_map->RetrieveMapHolder(io_position);
}

IMap::MapHolder Level::ExtractComponent(Position& io_position)
{
	return m_map->ExtractComponent(io_position);
}

IMap::MapHolder Level::AddComponent(IMap::MapHolder i_mapHolder, Position& io_position)
{
	return m_map->AddComponent(std::move(i_mapHolder), io_position);
}

void Level::IncreaseScore()
{
	if (++m_score == m_objectiveScore)
	{
		utils::Access<SignalKey>(sig_onFinishedLevel).Emit();
	}
}

void Level::DecreaseScore()
{
	if (m_score > 0)
	{
		m_score--;
	}
}

void Level::ResetScore()
{
	m_score = 0;
}

size_t Level::GetScore() const
{
	return m_score;
}

void Level::SetObjectiveScore(size_t i_objectiveScore)
{
	m_objectiveScore = i_objectiveScore;
}
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

void Level::OnShow() const
{
	m_map->OnShow();
}

void Level::OnHide() const
{
	m_map->OnHide();
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

IMap::Result Level::CheckValidMap()
{
	return m_map->CheckValidMap();
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
	m_score--;
}

void Level::ResetScore()
{
	m_score = 0;
}

long long Level::GetScore() const
{
	return m_score;
}

bool Level::Respawn()
{
	return m_allowedRespawns-- > 0;
}

void Level::SetObjectiveScore(long long i_objectiveScore)
{
	m_objectiveScore = i_objectiveScore;
}

void Level::SetAllowedRespawns(int i_allowedRespawns)
{
	m_allowedRespawns = i_allowedRespawns;
}

IMap& Level::GetMap() const
{
	return *m_map;
}

void Level::SetDebugEnable(bool i_enable)
{
	m_debugEnabled = i_enable;
}

bool Level::IsDebugEnabled() const
{
	return m_debugEnabled;
}
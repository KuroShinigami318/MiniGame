#include "stdafx.h"
#include "GameplaySystem/Level.h"
#include "Components/IProgressComponent.h"
#include "TimerDelayer.h"

namespace
{
constexpr const float k_finishedDelayerInMilliseconds = 3000.f;
}

Level::Level(utils::unique_ref<IMap> i_map)
	: IUIComponent(i_map->GetUIContext())
	, m_map(std::move(i_map))
	, m_progressComponent(nullptr)
	, m_timerDelayer(utils::make_unique<utils::TimerDelayer>(k_finishedDelayerInMilliseconds))
{
	m_timerDelayerConnection = m_timerDelayer->sig_onExpired.Connect(
	[this]()
	{
		utils::async(m_uiContext.thisFrameQueue, [this]()
		{
			utils::Access<SignalKey>(sig_onFinishedLevel).Emit();
		});
	});
	m_timerDelayer->CreateTimerThread();
	m_timerDelayer->Stop();
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

size_t Level::GetWidth() const
{
	return m_map->GetWidth();
}

size_t Level::GetHeight() const
{
	return m_map->GetHeight();
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
		m_timerDelayer->Reset();
		utils::async(m_uiContext.thisFrameQueue, &IProgressComponent::UpdateProgress, m_progressComponent, -1.f);
	}
	m_progressComponent->UpdateProgress(static_cast<float>(m_score) / static_cast<float>(m_objectiveScore) * m_progressComponent->GetMaxProgress());
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

void Level::AttachProgressComponent(IProgressComponent& i_progressComponent)
{
	m_progressComponent = &i_progressComponent;
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
#include "stdafx.h"
#include "GameplaySystem/MapSystem.h"

#include "AsyncScopedHelper.h"
#include "Components/ProgressComponent.h"
#include "Components/ScoresComponent.h"
#include "Control/IGameControl.h"
#include "GameplaySystem/ILevel.h"
#include "system_clock.h"
#include "UI/IMap.h"
#include "UI/IUIManager.h"

MapSystem::MapSystem(IGameControl& i_gameControl, IUIManager& i_uiManager, const utils::SystemClock& i_systemClock, utils::IMessageQueue& i_thisFrameQueue)
	: m_gameControl(i_gameControl)
	, m_uiManager(i_uiManager)
	, m_thisFrameQueue(i_thisFrameQueue)
	, m_progressComponent(utils::make_unique<ProgressComponent>(m_uiManager.GetUIContext()))
	, m_asyncScopedHelper(utils::make_unique<utils::AsyncScopedHelper>())
{
	m_updateConnection = i_systemClock.sig_onTick.Connect(&MapSystem::Update, this);
}

MapSystem::~MapSystem() = default;

void MapSystem::SetLevel(std::unique_ptr<ILevel> i_level)
{
	m_progressComponent->UpdateProgress(0.0f);
	i_level->AttachProgressComponent(*m_progressComponent);
	m_onFinishedLevelConnection = i_level->sig_onFinishedLevel.Connect(&MapSystem::OnFinishedLevel, this);
	if (IMap* map = dynamic_cast<IMap*>(i_level.release()))
	{
		OnMapChanged(std::unique_ptr<IMap>(map));
	}
	else
	{
		delete map;
	}
}

void MapSystem::OnMapChanged(std::unique_ptr<IMap> i_map)
{
	if (m_map)
	{
		m_uiManager.UnregisterUIComponent(*m_map).assertSuccess();
		m_uiManager.UnregisterUIComponent(*m_scoresComponent).assertSuccess();
		m_uiManager.UnregisterUIComponent(*m_progressComponent).assertSuccess();
	}
	m_scoresComponent.reset();
	m_map = std::move(i_map);
	if (m_map)
	{
		m_progressComponent->UpdateMaxProgress(static_cast<float>(m_map->GetWidth()));
		m_scoresComponent = std::make_unique<ScoresComponent>(m_map->GetUIContext(), dynamic_cast<ILevel&>(*m_map));
		m_uiManager.RegisterUIComponent(*m_map).assertSuccess();
		m_uiManager.RegisterUIComponent(*m_scoresComponent).assertSuccess();
		m_uiManager.RegisterUIComponent(*m_progressComponent).assertSuccess();
	}
}

void MapSystem::OnFinishedLevel()
{
}

void MapSystem::Update(float i_elapsed)
{

}
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
#include "UI/Screens/GameplayWindow.h"

MapSystem::MapSystem(IGameControl& i_gameControl, IUIManager& i_uiManager)
	: m_gameControl(i_gameControl)
	, m_uiManager(i_uiManager)
	, m_gameplayWindow(nullptr)
	, m_asyncScopedHelper(utils::make_unique<utils::AsyncScopedHelper>())
{
	m_updateConnection = m_uiManager.GetUIContext().systemClock.sig_onTick.Connect(&MapSystem::Update, this);
}

MapSystem::~MapSystem() = default;

void MapSystem::SetLevel(std::unique_ptr<ILevel> i_level)
{
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
	m_asyncScopedHelper->StartOptionalTask(m_uiManager.GetUIContext().thisFrameQueue,
		[this](std::unique_ptr<IMap> i_map)
		{
			ILevel& level = dynamic_cast<ILevel&>(*i_map);
			utils::unique_ref<ProgressComponent> progressComponent = utils::make_unique<ProgressComponent>(m_uiManager.GetUIContext(), static_cast<float>(i_map->GetWidth()));
			GameplayWindow gameplayWindow(m_uiManager.GetUIContext(), std::move(i_map));
			m_gameplayWindow = &gameplayWindow;
			m_gameplayWindow->AddUIComponent(utils::make_unique<ScoresComponent>(m_uiManager.GetUIContext(), level));
			IProgressComponent& progressComponentRef = *progressComponent;
			m_gameplayWindow->AddUIComponent(std::move(progressComponent));
			level.AttachProgressComponent(progressComponentRef);
			m_gameplayWindow->Open();
		}, std::move(i_map));
}

void MapSystem::OnFinishedLevel()
{
	m_gameplayWindow->Close();
}

void MapSystem::Update(float i_elapsed)
{
	m_asyncScopedHelper->CleanTasks();
}
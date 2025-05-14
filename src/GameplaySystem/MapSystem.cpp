#include "stdafx.h"
#include "Control/IGameControl.h"
#include "GameplaySystem/MapSystem.h"
#include "GameplaySystem/ILevel.h"
#include "system_clock.h"
#include "UI/IUIManager.h"
#include "UI/IMap.h"

MapSystem::MapSystem(IGameControl& i_gameControl, IUIManager& i_uiManager, const utils::SystemClock& i_systemClock)
	: m_gameControl(i_gameControl)
	, m_uiManager(i_uiManager)
{
	m_updateConnection = i_systemClock.sig_onTick.Connect(&MapSystem::Update, this);
}

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
	if (m_map)
	{
		m_uiManager.UnregisterUIComponent(*m_map).assertSuccess();
	}
	m_map = std::move(i_map);
	if (m_map)
	{
		m_uiManager.RegisterUIComponent(*m_map).assertSuccess();
	}
}

void MapSystem::OnFinishedLevel()
{
	m_map.reset();
}

void MapSystem::Update(float i_elapsed)
{

}
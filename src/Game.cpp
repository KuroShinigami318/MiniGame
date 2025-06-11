#include "stdafx.h"
#include "Game.h"
#include "Control/GameControl.h"
#include "GameplaySystem/LevelSystem.h"
#include "GameplaySystem/MapSystem.h"
#include "IInputDevice.h"
#include "UI/UIManager.h"
#include "UI/Map.h"
#include "system_clock.h"

Game::Game(utils::MessageSink_mt& i_nextFrameQueue, utils::MessageSink& i_thisFrameQueue)
	: m_nextFrameQueue(i_nextFrameQueue)
	, m_thisFrameQueue(i_thisFrameQueue)
	, m_systemClock(new utils::SystemClock())
	, m_gameControl(new GameControl())
	, m_uiManager(new UIManager())
	, m_levelSystem(new LevelSystem(*m_gameControl, *m_systemClock, m_thisFrameQueue, m_nextFrameQueue))
	, m_mapSystem(new MapSystem(*m_gameControl, *m_uiManager, *m_systemClock, m_thisFrameQueue))
{
	m_connections.push_back(m_levelSystem->sig_onLevelChanged.Connect(&MapSystem::SetLevel, m_mapSystem.get()));
}

Game::~Game() = default;

void Game::RegisterInputDevice(IInputDevice& i_inputDevice)
{
	m_gameControl->RegisterInputDevice(i_inputDevice);
}

void Game::UnregisterInputDevice(IInputDevice& i_inputDevice)
{
	m_gameControl->UnregisterInputDevice(i_inputDevice);
}

void Game::FramePrologue(float i_elapsed)
{
	m_systemClock->Update(i_elapsed);
}

void Game::FrameEpilogue(RendererT& o_renderStream) const
{
	m_uiManager->Render(o_renderStream);
}

void Game::OnReload()
{

}

void Game::OnExit()
{

}
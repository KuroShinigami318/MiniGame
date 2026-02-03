#include "stdafx.h"
#include "Game.h"
#include "Control/GameControl.h"
#include "GameplaySystem/LevelSystem.h"
#include "GameplaySystem/MapSystem.h"
#include "IInputDevice.h"
#include "UI/UIManager.h"
#include "UI/WindowManager.h"
#include "UI/Screens/SplashscreenWindow.h"
#include "UI/Map.h"
#include "system_clock.h"

Game::Game(utils::MessageSink_mt& i_nextFrameQueue, utils::MessageSink& i_thisFrameQueue, utils::IRecursiveControl& i_recursiveControl)
	: m_nextFrameQueue(i_nextFrameQueue)
	, m_thisFrameQueue(i_thisFrameQueue)
	, m_recursiveControl(i_recursiveControl)
	, m_systemClock(new utils::SystemClock())
	, m_gameControl(new GameControl())
	, m_windowManager(new WindowManager())
	, m_uiManager(new UIManager(i_thisFrameQueue, i_nextFrameQueue, i_recursiveControl, *m_systemClock, *m_windowManager))
	, m_levelSystem(new LevelSystem(*m_gameControl, *m_uiManager))
	, m_mapSystem(new MapSystem(*m_gameControl, *m_uiManager))
{
	m_connections.push_back(m_levelSystem->sig_onLevelChanged.Connect(&MapSystem::SetLevel, m_mapSystem.get()));
	utils::async(m_thisFrameQueue, &Game::Run, this);
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
	m_windowManager->CloseAllWindows();
	m_levelSystem->StartLevelGeneration();
	utils::async(m_thisFrameQueue, &Game::Run, this);
}

void Game::OnExit()
{
	m_windowManager->CloseAllWindows();
}

void Game::Run()
{
	SplashscreenWindow splashscreenWindow(m_uiManager->GetUIContext(), 50.f);
	splashscreenWindow.Open();
}

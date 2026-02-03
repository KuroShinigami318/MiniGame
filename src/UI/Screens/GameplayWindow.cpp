#include "stdafx.h"
#include "UI/Screens/GameplayWindow.h"
#include "UI/IMap.h"

GameplayWindow::GameplayWindow(const UIContext& i_uiContext, utils::unique_ref<IMap> i_map)
	: WindowBase(i_uiContext)
{
	AddUIComponent(std::move(i_map));
}

utils::unique_ref<IComponent> GameplayWindow::Clone()
{
	utils::unique_ref<IComponent> clonedComponent = utils::make_unique<GameplayWindow>(m_uiContext, utils::dynamic_unique_cast<IMap>(std::move(m_uiComponents.back())));
	m_uiComponents.pop_back();
	return clonedComponent;
}

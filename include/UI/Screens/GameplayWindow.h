#pragma once

#include "UI/WindowBase.h"

class IMap;

class GameplayWindow : public WindowBase
{
public:
	GameplayWindow(const UIContext& i_uiContext, utils::unique_ref<IMap> i_map);
	utils::unique_ref<IComponent> Clone() override;
};
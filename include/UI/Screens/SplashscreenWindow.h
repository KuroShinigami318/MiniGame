#pragma once

#include "UI/WindowBase.h"

class SplashscreenWindow : public WindowBase
{
public:
	SplashscreenWindow(const UIContext& i_uiContext, const float& i_barWidth = 100.f);
	utils::unique_ref<IComponent> Clone() override;

private:
	const float m_barWidth;
};
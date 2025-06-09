#pragma once
#include "UI/IUIComponent.h"

class ILevel;

class ScoresComponent final : public IUIComponent
{
public:
	ScoresComponent(const UIContext& i_uiContext, ILevel& i_level);
	utils::unique_ref<IComponent> Clone() override;
	void Render(RendererT& o_renderStream) const override;

private:
	ILevel& m_level;
};
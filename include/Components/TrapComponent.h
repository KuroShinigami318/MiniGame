#pragma once
#include "GameplaySystem/IBreakable.h"
#include "GameplaySystem/ICollidable.h"
#include "UI/IUIComponent.h"

class ILevel;

class TrapComponent : public IUIComponent, public ICollidable, public IBreakable
{
public:
	TrapComponent(const UIContext& i_uiContext, ILevel& i_level);
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;
	void OnCollision(const ICollidable& i_other) override;

private:
	ILevel& m_level;
};
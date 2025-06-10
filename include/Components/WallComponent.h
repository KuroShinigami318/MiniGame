#pragma once
#include "GameplaySystem/ICollidable.h"
#include "UI/IUIComponent.h"

class ILevel;

class WallComponent : public IUIComponent, public ICollidable
{
public:
	WallComponent(const UIContext& i_uiContext, ILevel& i_level);
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;
	void OnCollision(const ICollidable& i_other) override;

private:
	ILevel& m_level;
};
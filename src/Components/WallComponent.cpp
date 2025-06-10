#include "stdafx.h"
#include "Components/WallComponent.h"
#include "GameplaySystem/ILevel.h"

WallComponent::WallComponent(const UIContext& i_uiContext, ILevel& i_level)
	: IUIComponent(i_uiContext)
	, m_level(i_level)
{
}

void WallComponent::Render(RendererT& o_renderStream) const
{
	o_renderStream << "|";
}

utils::unique_ref<IComponent> WallComponent::Clone()
{
	return utils::make_unique<WallComponent>(m_uiContext, m_level);
}

void WallComponent::OnCollision(const ICollidable& i_other)
{
}
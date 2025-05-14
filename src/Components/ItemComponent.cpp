#include "stdafx.h"
#include "Components/ItemComponent.h"
#include "UI/UIHelper.h"

void ItemComponent::Render(RendererT& o_renderStream) const
{
	o_renderStream << FormatText("#", utils::Log::TextFormat{ utils::Log::TextStyle::Italic, utils::RGBColor{255, 255, 0} });;
}

utils::unique_ref<IComponent> ItemComponent::Clone()
{
	return utils::make_unique<ItemComponent>(*this);
}
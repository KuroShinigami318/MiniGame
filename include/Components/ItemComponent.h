#pragma once
#include "UI/IUIComponent.h"

class ItemComponent : public IUIComponent
{
public:
	ItemComponent() = default;
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;
};
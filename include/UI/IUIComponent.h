#pragma once
#include "Components/IComponent.h"
#include "RendererType.h"

class IUIComponent : virtual public IComponent
{
public:
	virtual void Render(RendererT& o_renderStream) const = 0;
};
#pragma once

#include "IUIComponent.h"

class IWindow : public IUIComponent
{
public:
	using IUIComponent::IUIComponent;
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual bool IsOpened() const = 0;
	virtual void AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent) = 0;
};
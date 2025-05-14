#pragma once
#include "IUIManager.h"
#include "RendererType.h"

class UIManager : public IUIManager
{
public:
	Result RegisterUIComponent(const IUIComponent&) override;
	Result UnregisterUIComponent(const IUIComponent&) override;
	void Render(RendererT& o_renderStream) const;

private:
	std::vector<const IUIComponent*> m_uiComponents;
};
#pragma once
#include "IUIManager.h"
#include "RendererType.h"

class UIManager : public IUIManager
{
public:
	UIManager(const UIContext& i_uiContext);
	Result RegisterUIComponent(const IUIComponent&) override;
	Result UnregisterUIComponent(const IUIComponent&) override;
	void Render(RendererT& o_renderStream) const;
	const UIContext& GetUIContext() const override;

private:
	std::vector<const IUIComponent*> m_uiComponents;
	utils::unique_ref<UIContext> m_uiContext;
};
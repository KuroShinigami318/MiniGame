#pragma once
#include "IUIManager.h"
#include "RendererType.h"

namespace utils
{
class IMessageQueue;
class IRecursiveControl;
struct SystemClock;
}

class WindowManager;
class IUIManager;

class UIManager : public IUIManager
{
public:
	UIManager(utils::IMessageQueue& i_thisFrameQueue, utils::IMessageQueue& i_nextFrameQueue, utils::IRecursiveControl& i_recursiveControl, const utils::SystemClock& i_systemClock, WindowManager& i_windowManager);
	Result RegisterUIComponent(const IUIComponent&) override;
	Result UnregisterUIComponent(const IUIComponent&) override;
	void Render(RendererT& o_renderStream) const;
	const UIContext& GetUIContext() const override;

private:
	std::vector<const IUIComponent*> m_uiComponents;
	utils::unique_ref<UIContext> m_uiContext;
};
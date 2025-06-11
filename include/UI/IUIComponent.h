#pragma once
#include "AsyncScopedHelper.h"
#include "UIContext.h"
#include "Components/IComponent.h"
#include "RendererType.h"

class IUIComponent : virtual public IComponent
{
public:
	IUIComponent(const UIContext& i_uiContext) : m_uiContext(i_uiContext) {}
	const UIContext& GetUIContext() const { return m_uiContext; }
	virtual void OnShow() const {}
	virtual void OnHide() const {}
	virtual void Render(RendererT& o_renderStream) const = 0;
	template <typename... Args>
	void StartOptionalTask(Args&&... args)
	{
		m_asyncScopedHelper.StartOptionalTask(std::forward<Args>(args)...);
	}

protected:
	UIContext m_uiContext;
	utils::AsyncScopedHelper m_asyncScopedHelper;
};
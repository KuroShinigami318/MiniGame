#include "stdafx.h"
#include "UI/WindowBase.h"
#include "UI/WindowManager.h"

void WindowBase::Open()
{
	m_uiContext.windowManager.RegisterWindow(*this);
	m_uiContext.recursiveControl.enter(HashObject(this)).assertSuccess();
}

void WindowBase::Close()
{
	m_uiContext.windowManager.UnregisterWindow(*this);
	m_uiContext.recursiveControl.exit(HashObject(this));
	m_currentVisibility.clear();
}

bool WindowBase::IsOpened() const
{
	return !m_currentVisibility.empty();
}

void WindowBase::OnShow() const
{
	m_currentVisibility.set(visibility_flag::visible);
	m_currentVisibility.unset(visibility_flag::invisible);
}

void WindowBase::OnHide() const
{
	m_currentVisibility.set(visibility_flag::invisible);
	m_currentVisibility.unset(visibility_flag::visible);
}

void WindowBase::AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent)
{
	m_uiComponents.push_back(std::move(i_uiComponent));
}

void WindowBase::Render(RendererT& o_renderStream) const
{
	for (const auto& uiComponent : m_uiComponents)
	{
		uiComponent->Render(o_renderStream);
	}
}

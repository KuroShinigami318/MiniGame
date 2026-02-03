#include "stdafx.h"
#include "UI/WindowManager.h"
#include "UI/IWindow.h"
#include "UI/IUIManager.h"

void WindowManager::RegisterWindow(IWindow& i_window)
{
	IUIManager& uiManager = i_window.GetUIContext().uiManager;
	if (!m_windows.empty())
	{
		uiManager.UnregisterUIComponent(*m_windows.back()).assertSuccess();
	}
	m_windows.push_back(&i_window);
	uiManager.RegisterUIComponent(i_window).assertSuccess();
}

void WindowManager::UnregisterWindow(IWindow& i_window)
{
	ASSERT(!m_windows.empty());
	IUIManager& uiManager = i_window.GetUIContext().uiManager;
	uiManager.UnregisterUIComponent(i_window).assertSuccess();
	if (m_windows.back() == &i_window)
	{
		m_windows.pop_back();
		while (!m_windows.empty() && !m_windows.back()->IsOpened())
		{
			m_windows.pop_back();
		}
		if (!m_windows.empty())
		{
			uiManager.RegisterUIComponent(*m_windows.back()).assertSuccess();
		}
	}
}

void WindowManager::CloseAllWindows()
{
	while (!m_windows.empty())
	{
		m_windows.back()->Close();
	}
}

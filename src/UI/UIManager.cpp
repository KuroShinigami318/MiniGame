#include "stdafx.h"
#include "UI/UIManager.h"
#include "UI/IUIComponent.h"
#include "DisplayInfo.h"

#include "details/platforms.h"
#if defined(USE_POSIX_API)
#include <sys/ioctl.h>
#endif

namespace
{
utils::unique_ref<DisplayInfo> GetDisplayInfo()
{
	int width = 0, height = 0;
#if defined(USE_WIN32_API)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	width = w.ws_col;
	height = w.ws_row;
#endif
	return utils::make_unique<DisplayInfo>(width, height);
}
}

UIManager::UIManager(utils::IMessageQueue& i_thisFrameQueue, utils::IMessageQueue& i_nextFrameQueue, utils::IRecursiveControl& i_recursiveControl, const utils::SystemClock& i_systemClock, WindowManager& i_windowManager)
	: m_uiContext(utils::make_unique<UIContext>(i_thisFrameQueue, i_nextFrameQueue, i_recursiveControl, i_systemClock, i_windowManager, *this))
	, m_displayInfo(::GetDisplayInfo())
{
}

UIManager::~UIManager() = default;

IUIManager::Result UIManager::RegisterUIComponent(const IUIComponent& i_uiComponent)
{
	auto componentFoundIt = std::find_if(m_uiComponents.begin(), m_uiComponents.end(),
		[&i_uiComponent](const IUIComponent* uiComponent)
		{
			return uiComponent == &i_uiComponent;
		});
	if (componentFoundIt != m_uiComponents.end())
	{
		return make_error<Error>(ErrorCode::AlreadyRegisteredComponent);
	}
	i_uiComponent.OnShow();
	m_uiComponents.push_back(&i_uiComponent);
	return utils::Ok();
}

IUIManager::Result UIManager::UnregisterUIComponent(const IUIComponent& i_uiComponent)
{
	auto componentFoundIt = std::find_if(m_uiComponents.begin(), m_uiComponents.end(),
		[&i_uiComponent](const IUIComponent* uiComponent)
		{
			return uiComponent == &i_uiComponent;
		});
	if (componentFoundIt == m_uiComponents.end())
	{
		return make_error<Error>(ErrorCode::UnregisteredComponent);
	}
	i_uiComponent.OnHide();
	m_uiComponents.erase(componentFoundIt);
	return utils::Ok();
}

void UIManager::Render(RendererT& o_renderStream) const
{
	for (const IUIComponent* uiComponent : m_uiComponents)
	{
		uiComponent->Render(o_renderStream);
	}
}

const UIContext& UIManager::GetUIContext() const
{
	return *m_uiContext;
}

const DisplayInfo& UIManager::GetDisplayInfo() const
{
	return *m_displayInfo;
}
#pragma once

namespace utils
{
class IMessageQueue;
class IRecursiveControl;
struct SystemClock;
}

class WindowManager;
class IUIManager;

struct UIContext
{
	utils::IMessageQueue& thisFrameQueue;
	utils::IMessageQueue& nextFrameQueue;
	utils::IRecursiveControl& recursiveControl;
	const utils::SystemClock& systemClock;
	WindowManager& windowManager;
	IUIManager& uiManager;
	UIContext(utils::IMessageQueue& i_thisFrameQueue
		, utils::IMessageQueue& i_nextFrameQueue
		, utils::IRecursiveControl& i_recursiveControl
		, const utils::SystemClock& i_systemClock
		, WindowManager& i_windowManager
		, IUIManager& i_uiManager)
		: thisFrameQueue(i_thisFrameQueue)
		, nextFrameQueue(i_nextFrameQueue)
		, recursiveControl(i_recursiveControl)
		, systemClock(i_systemClock)
		, windowManager(i_windowManager)
		, uiManager(i_uiManager)
	{
	}
};
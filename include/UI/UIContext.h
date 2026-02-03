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
};
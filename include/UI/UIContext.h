#pragma once

namespace utils
{
class IMessageQueue;
class IRecursiveControl;
}

struct UIContext
{
	utils::IMessageQueue& thisFrameQueue;
	utils::IMessageQueue& nextFrameQueue;
   utils::IRecursiveControl& recursiveControl;
};
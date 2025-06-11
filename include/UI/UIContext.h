#pragma once

namespace utils
{
class IMessageQueue;
}

struct UIContext
{
	utils::IMessageQueue& thisFrameQueue;
	utils::IMessageQueue& nextFrameQueue;
};
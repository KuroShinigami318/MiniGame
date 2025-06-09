#pragma once

class IBreakable
{
protected:
	struct SignalKey;

public:
	virtual ~IBreakable() = default;
	utils::Signal_public<void(IBreakable&), SignalKey> sig_onBroken;
};
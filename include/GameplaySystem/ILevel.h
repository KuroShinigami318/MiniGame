#pragma once

class ILevel
{
protected:
	struct SignalKey;

public:
	virtual ~ILevel() = default;

public:
	utils::Signal_public<void(), SignalKey> sig_onFinishedLevel;
};
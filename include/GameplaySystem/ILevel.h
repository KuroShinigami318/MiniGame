#pragma once

class ILevel
{
protected:
	struct SignalKey;

public:
	virtual ~ILevel() = default;
	virtual void IncreaseScore() = 0;
	virtual void DecreaseScore() = 0;
	virtual void ResetScore() = 0;
	virtual size_t GetScore() const = 0;

public:
	utils::Signal_public<void(), SignalKey> sig_onFinishedLevel;
};
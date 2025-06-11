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
	virtual long long GetScore() const = 0;
	virtual bool Respawn() = 0;
	virtual void SetDebugEnable(bool i_enable) = 0;
	virtual bool IsDebugEnabled() const = 0;

public:
	utils::Signal_public<void(), SignalKey> sig_onFinishedLevel;
};
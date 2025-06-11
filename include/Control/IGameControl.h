#pragma once

class IGameControl
{
protected:
	struct SignalKey;

public:
	DeclareScopedEnum(ControlType, uint8_t,
		MoveUp,
		MoveDown,
		MoveRight,
		MoveLeft,
		ToggleDebug);
public:
	virtual ~IGameControl() = default;
	utils::Signal_public<void(ControlType), SignalKey> sig_onControlReceived;
};

DefineScopeEnumOperatorImpl(ControlType, IGameControl)
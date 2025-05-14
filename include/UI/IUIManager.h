#pragma once

class IUIComponent;

class IUIManager
{
public:
	DeclareScopedEnum(ErrorCode, uint8_t,
		UnregisteredComponent,
		AlreadyRegisteredComponent)
	using Error = utils::Error<ErrorCode>;
	using Result = utils::Result<void, Error>;

public:
	virtual ~IUIManager() = default;
	virtual Result RegisterUIComponent(const IUIComponent&) = 0;
	virtual Result UnregisterUIComponent(const IUIComponent&) = 0;
};
DefineScopeEnumOperatorImpl(ErrorCode, IUIManager)
#pragma once

class IComponent
{
public:
	virtual ~IComponent() = default;
	virtual utils::unique_ref<IComponent> Clone() = 0;
};
#pragma once
#include "IComponent.h"

class IPlayer : virtual public IComponent
{
public:
	virtual ~IPlayer() = default;
};
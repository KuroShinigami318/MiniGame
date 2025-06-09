#pragma once

class ICollidable
{
public:
	virtual ~ICollidable() = default;
	virtual void OnCollision(const ICollidable& other) = 0;
};
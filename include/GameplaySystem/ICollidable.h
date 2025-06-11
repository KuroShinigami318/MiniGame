#pragma once

class ICollidable
{
public:
	virtual ~ICollidable() = default;
	virtual bool IsCollisionEnabled() const { return true; }
	virtual void OnCollision(const ICollidable& other) = 0;
};
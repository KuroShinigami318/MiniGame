#pragma once
#include "Components/IComponent.h"
#include "UI/UiTypes.h"

namespace utils
{
struct SystemClock;
}

class MovableComponent : virtual public IComponent
{
private:
	struct SignalKey;

public:
	MovableComponent(const Vec2f& i_veclocity, const utils::SystemClock& i_systemClock);
	void ResetMovement(const Vec2f& i_veclocity);

protected:
	virtual void OnUpdate(float i_delta);

public:
	utils::Signal_public<void(Vec2f), SignalKey> sig_onMoved;

protected:
	Vec2f m_veclocity;
	Vec2f m_distanceMoved;
	utils::Connection m_onUpdateConnection;
};
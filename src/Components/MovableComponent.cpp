#include "stdafx.h"
#include "Components/MovableComponent.h"
#include "system_clock.h"

MovableComponent::MovableComponent(const Vec2f& i_veclocity, const utils::SystemClock& i_systemClock)
	: m_veclocity(i_veclocity)
	, m_distanceMoved(0)
{
	m_onUpdateConnection = i_systemClock.sig_onTick.Connect(&MovableComponent::OnUpdate, this);
}

void MovableComponent::OnUpdate(float i_delta)
{
	m_distanceMoved += m_veclocity /** i_delta*/;
	utils::Access<SignalKey>(sig_onMoved).Emit(m_distanceMoved);
}

void MovableComponent::ResetMovement(const Vec2f& i_veclocity)
{
	m_distanceMoved = i_veclocity;
}
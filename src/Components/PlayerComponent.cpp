#include "stdafx.h"
#include "Components/PlayerComponent.h"
#include "Control/IGameControl.h"
#include "UI/UIHelper.h"

PlayerComponent::PlayerComponent(const Vec2f& i_veclocity, const IGameControl& i_gameControl, const utils::SystemClock& i_systemClock)
	: MovableComponent(0, i_systemClock)
	, m_movementVeclocity(i_veclocity)
	, m_gameControl(i_gameControl)
	, m_systemClock(i_systemClock)
{
	m_onControlReceivedConnection = m_gameControl.sig_onControlReceived.Connect(&PlayerComponent::OnControlReceived, this);
}

void PlayerComponent::Render(RendererT& o_renderStream) const
{
	o_renderStream << FormatText("P", utils::Log::TextFormat{ utils::Log::TextStyle::Bold, utils::RGBColor{255, 0, 255} });
}

utils::unique_ref<IComponent> PlayerComponent::Clone()
{
	return utils::make_unique<PlayerComponent>(m_movementVeclocity, m_gameControl, m_systemClock);
}

void PlayerComponent::OnControlReceived(const IGameControl::ControlType& i_controlType)
{
	switch (i_controlType)
	{
	case IGameControl::ControlType::MoveUp:
	{
		m_veclocity[1] = -m_movementVeclocity[1];
	}
	break;
	case IGameControl::ControlType::MoveDown:
	{
		m_veclocity[1] = m_movementVeclocity[1];
	}
	break;
	case IGameControl::ControlType::MoveLeft:
	{
		m_veclocity[0] = -m_movementVeclocity[0];
	}
	break;
	case IGameControl::ControlType::MoveRight:
	{
		m_veclocity[0] = m_movementVeclocity[0];
	}
	break;
	default: break;
	}
}

void PlayerComponent::OnUpdate(float i_delta)
{
	MovableComponent::OnUpdate(i_delta);
	m_veclocity = 0;
}
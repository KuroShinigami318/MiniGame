#pragma once
#include "IPlayer.h"
#include "MovableComponent.h"
#include "UI/IUIComponent.h"
#include "Control/IGameControl.h"

constexpr const Vec2f k_defaultVeclocity(1.f);

class PlayerComponent : public IPlayer, public MovableComponent, public IUIComponent
{
public:
	PlayerComponent(const Vec2f& i_veclocity, const IGameControl& i_gameControl, const utils::SystemClock& i_systemClock);
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;

private:
	void OnControlReceived(const IGameControl::ControlType& i_controlType);
	void OnUpdate(float i_delta) override;

private:
	Vec2f m_movementVeclocity;
	const IGameControl& m_gameControl;
	const utils::SystemClock& m_systemClock;
	utils::Connection m_onControlReceivedConnection;
};
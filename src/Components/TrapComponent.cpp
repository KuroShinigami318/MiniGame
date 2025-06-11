#include "stdafx.h"
#include "Components/TrapComponent.h"
#include "GameplaySystem/ILevel.h"
#include "UI/UIHelper.h"

TrapComponent::TrapComponent(const UIContext& i_uiContext, ILevel& i_level)
	: IUIComponent(i_uiContext)
	, m_level(i_level)
{
}

void TrapComponent::Render(RendererT& o_renderStream) const
{
	std::string trap = ".";
	if (m_level.IsDebugEnabled())
	{
		trap = FormatText(".", utils::Log::TextFormat{ utils::Log::TextStyle::Bold, utils::RGBColor{0, 0, 255} });
	}
	o_renderStream << trap;
}

utils::unique_ref<IComponent> TrapComponent::Clone()
{
	return utils::make_unique<TrapComponent>(m_uiContext, m_level);
}

void TrapComponent::OnCollision(const ICollidable& i_other)
{
	utils::Access<SignalKey>(sig_onBroken).Emit(*this);
}
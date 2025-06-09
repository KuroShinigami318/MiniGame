#include "stdafx.h"
#include "Components/ItemComponent.h"
#include "Components/IPlayer.h"
#include "GameplaySystem/ILevel.h"
#include "UI/UIHelper.h"

ItemComponent::ItemComponent(const UIContext& i_uiContext, ILevel& i_level)
	: IUIComponent(i_uiContext)
	, m_level(i_level)
{
}

void ItemComponent::Render(RendererT& o_renderStream) const
{
	o_renderStream << FormatText("#", utils::Log::TextFormat{ utils::Log::TextStyle::Italic, utils::RGBColor{255, 255, 0} });
}

utils::unique_ref<IComponent> ItemComponent::Clone()
{
	return utils::make_unique<ItemComponent>(m_uiContext, m_level);
}

void ItemComponent::OnCollision(const ICollidable& i_other)
{
	if (const IPlayer* player = dynamic_cast<const IPlayer*>(&i_other))
	{
		m_level.IncreaseScore();
		utils::Access<SignalKey>(sig_onBroken).Emit(*this);
	}
}
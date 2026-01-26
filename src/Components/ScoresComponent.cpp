#include "stdafx.h"
#include "Components/ScoresComponent.h"

#include "GameplaySystem/ILevel.h"
#include "UI/UIHelper.h"

ScoresComponent::ScoresComponent(const UIContext& i_uiContext, ILevel& i_level)
	: IUIComponent(i_uiContext)
	, m_level(i_level)
{
}

utils::unique_ref<IComponent> ScoresComponent::Clone()
{
	return utils::make_unique<ScoresComponent>(m_uiContext, m_level);
}

void ScoresComponent::Render(RendererT& o_renderStream) const
{
	std::string scores = FormatText(utils::Format("Your scores is: {}", m_level.GetScore()).c_str(), utils::Log::TextFormat{ utils::Log::TextStyle::Italic, utils::RGBColor{0, 255, 0} });
	if (m_level.GetScore() < 0)
	{
		scores = FormatText(utils::Format("You are lost!").c_str(), utils::Log::TextFormat{ utils::Log::TextStyle::Italic, utils::RGBColor{255, 0, 0} });
	}
	o_renderStream << scores;
	o_renderStream << std::endl << "\033[1G";
}
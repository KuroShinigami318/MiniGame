#include "stdafx.h"
#include "Components/ProgressComponent.h"
#include "UI/UIHelper.h"
#include "system_clock.h"

ProgressComponent::ProgressComponent(const UIContext& i_uiContext, const float& i_maxProgress)
	: IUIComponent(i_uiContext)
	, m_progress(0.f)
	, m_maxProgress(i_maxProgress)
	, m_startPoint(0.f)
	, m_barWidth(0.f)
	, m_speed(100.f)
{
	m_connection = m_uiContext.systemClock.sig_onTick.Connect(
		[this](float delta)
		{
			if (m_progress < 0)
			{
				m_startPoint = int(m_startPoint + delta * m_speed) % (int)m_maxProgress;
			}
		});
}

void ProgressComponent::UpdateProgress(const float i_progress)
{
	m_progress = std::clamp(i_progress, -1.f, m_maxProgress);
	m_barWidth = m_progress >= 0 ? m_progress : m_maxProgress / 3;
	m_startPoint = 0.f;
}

void ProgressComponent::UpdateMaxProgress(const float& i_maxProgress)
{
	m_maxProgress = i_maxProgress;
}

const float& ProgressComponent::GetMaxProgress() const
{
	return m_maxProgress;
}

void ProgressComponent::Render(RendererT& o_renderStream) const
{
	utils::Log::TextFormat progressStyle{ utils::Log::TextStyle::Italic, utils::RGBColor{0, 255, 0} };
	o_renderStream << FormatText("[", progressStyle);
	for (float i = 0; i < m_maxProgress; ++i)
	{
		if (int(i + m_maxProgress - m_startPoint) % (int)m_maxProgress < m_barWidth)
		{
			o_renderStream << FormatText("##", progressStyle);
		}
		else
		{
			o_renderStream << "..";
		}
	}
	o_renderStream << FormatText("]", progressStyle);
	if (m_progress < 0)
	{
		o_renderStream << FormatText(" Loading", progressStyle);
		return;
	}
	o_renderStream << FormatText(utils::Format(" {}%", (m_progress / m_maxProgress) * 100.f).c_str(), progressStyle);
	o_renderStream << std::endl << "\033[1G";
}

utils::unique_ref<IComponent> ProgressComponent::Clone()
{
	return utils::make_unique<ProgressComponent>(m_uiContext, m_maxProgress);
}
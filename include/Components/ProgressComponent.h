#pragma once
#include "IProgressComponent.h"
#include "UI/IUIComponent.h"

class ProgressComponent : public IProgressComponent, public IUIComponent
{
public:
	ProgressComponent(const UIContext& i_uiContext, const float& i_maxProgress = 100.f);
	void UpdateProgress(const float i_progress) override;
	void UpdateMaxProgress(const float& i_maxProgress) override;
	const float& GetMaxProgress() const override;
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;

private:
	float m_progress;
	float m_maxProgress;
	float m_startPoint;
	float m_barWidth;
	float m_speed;
	utils::Connection m_connection;
};
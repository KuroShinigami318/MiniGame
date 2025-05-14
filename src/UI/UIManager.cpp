#include "stdafx.h"
#include "UI/UIManager.h"
#include "UI/IUIComponent.h"

IUIManager::Result UIManager::RegisterUIComponent(const IUIComponent& i_uiComponent)
{
	auto componentFoundIt = std::find_if(m_uiComponents.begin(), m_uiComponents.end(),
		[&i_uiComponent](const IUIComponent* uiComponent)
		{
			return uiComponent == &i_uiComponent;
		});
	if (componentFoundIt != m_uiComponents.end())
	{
		return make_error<Error>(ErrorCode::AlreadyRegisteredComponent);
	}
	m_uiComponents.push_back(&i_uiComponent);
	return utils::Ok();
}

IUIManager::Result UIManager::UnregisterUIComponent(const IUIComponent& i_uiComponent)
{
	auto componentFoundIt = std::find_if(m_uiComponents.begin(), m_uiComponents.end(),
		[&i_uiComponent](const IUIComponent* uiComponent)
		{
			return uiComponent == &i_uiComponent;
		});
	if (componentFoundIt == m_uiComponents.end())
	{
		return make_error<Error>(ErrorCode::UnregisteredComponent);
	}
	m_uiComponents.erase(componentFoundIt);
	return utils::Ok();
}

void UIManager::Render(RendererT& o_renderStream) const
{
	for (const IUIComponent* uiComponent : m_uiComponents)
	{
		uiComponent->Render(o_renderStream);
	}
}
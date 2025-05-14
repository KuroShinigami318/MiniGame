#include "stdafx.h"
#include "Control/GameControl.h"
#include "IInputDevice.h"

GameControl::GameControl()
	: m_inputControlMapped({{"w", ControlType::MoveUp}, { "s", ControlType::MoveDown }, { "a", ControlType::MoveLeft } , { "d", ControlType::MoveRight } })
	, m_inputDevice(nullptr)
{
}

void GameControl::RegisterInputDevice(IInputDevice& i_inputDevice)
{
	if (m_inputDevice != &i_inputDevice)
	{
		m_inputDevice = &i_inputDevice;
	}
	m_onInputConnection = m_inputDevice->sig_onInput.Connect(&GameControl::OnInputReceived, this);
}

void GameControl::UnregisterInputDevice(IInputDevice& i_inputDevice)
{
	if (m_inputDevice == &i_inputDevice)
	{
		m_inputDevice = nullptr;
	}
}

bool GameControl::OnInputReceived(std::string i_input)
{
	std::string inputString;
	std::transform(i_input.begin(), i_input.end(), std::back_inserter(inputString),
		[](const char& c)
		{
			return std::tolower(c);
		});
	if (auto mappedIt = m_inputControlMapped.find(inputString); mappedIt != m_inputControlMapped.end())
	{
		utils::Access<SignalKey>(sig_onControlReceived).Emit(mappedIt->second);
	}
	return true;
}
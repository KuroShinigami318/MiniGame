#pragma once
#include "IGameControl.h"

class IInputDevice;

class GameControl : public IGameControl
{
public:
	GameControl();
	void RegisterInputDevice(IInputDevice& i_inputDevice);
	void UnregisterInputDevice(IInputDevice& i_inputDevice);

private:
	bool OnInputReceived(std::string i_input);

private:
	struct InputDeviceHolder
	{
		IInputDevice& device;
		utils::Connection connection;
	};

private:
	std::unordered_map<std::string, ControlType> m_inputControlMapped;
	std::unordered_map<IInputDevice*, InputDeviceHolder> m_inputDevices;
	utils::Connection m_onInputConnection;
};
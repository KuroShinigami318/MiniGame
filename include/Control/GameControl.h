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
	std::unordered_map<std::string, ControlType> m_inputControlMapped;
	IInputDevice* m_inputDevice;
	utils::Connection m_onInputConnection;
};
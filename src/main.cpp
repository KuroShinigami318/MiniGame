#include "stdafx.h"
#include "Application.h"
#include "HeartBeats.h"
#include "SystemInputDevice.h"

int main()
{
	Application application(new utils::HeartBeats(60, utils::BPS()));
	utils::unique_ref<IInputDevice> inputDevice(new SystemInputDevice(SystemInputDevice::InputMode::Char, application.GetNextFrameMessageQueue()));
	application.RegisterInputDevice(*inputDevice);
	application.Execute();
	application.UnregisterInputDevice(*inputDevice);
	return 0;
}
#pragma once

class IWindow;

class WindowManager
{
public:
	void RegisterWindow(IWindow& i_window);
	void UnregisterWindow(IWindow& i_window);
	void CloseAllWindows();

private:
	std::vector<IWindow*> m_windows;
};
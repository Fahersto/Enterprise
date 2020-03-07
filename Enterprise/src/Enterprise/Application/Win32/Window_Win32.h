#pragma once
#include "Enterprise/Application/Window.h"

namespace Enterprise::Platform {

	class Win32Window : public Window {
	public:
		// Interface Stuff ----------------------------------------------------------
		Win32Window(const WindowSettings& settings);
		virtual ~Win32Window();

		unsigned int GetWidth() override { return m_Settings.Width; };
		unsigned int GetHeight() override { return m_Settings.Height; };

	private:
		// Interface Stuff ----------------------------------------------------------
		WindowSettings m_Settings;

		// Platform Stuff -----------------------------------------------------------
		HWND hWnd; // Window handle
	};
}
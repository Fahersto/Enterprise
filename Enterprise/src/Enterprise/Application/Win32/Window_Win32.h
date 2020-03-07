#pragma once
#include "Enterprise/Application/Window.h"

namespace Enterprise {

	class Window_Win32 : public Window {
	public:
		Window_Win32(const WindowSettings& settings);
		virtual ~Window_Win32() override;

	private:
		HWND hWnd; // Win32 Window handle
	};
}
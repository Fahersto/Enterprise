#pragma once

/* Window
	The class that represents the game window.  While there will generally only be one window per game,
	it is possible to create multiple windows in a single application.  The code to create a Window is
	called in the client application.
*/

#include "EP_PCH.h"
#include "Core.h"

//#include "Enterprise/Events/Dispatcher.h"

namespace Enterprise {
	struct WindowSettings
	{
		std::wstring Title;
		unsigned int Width;
		unsigned int Height;

		// TODO: Add window icon support
		WindowSettings(const std::wstring& title = L"Enterprise",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};
	
	class Window
	{
	public:

		// TODO: Add fullscreen, vsync support here

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual ~Window() {}

		static Window* Create(const WindowSettings& attributes = WindowSettings());
	};
}
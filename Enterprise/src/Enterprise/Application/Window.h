#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{
	/* The interface to the game window.  Contains functions for creating, destroying, and manipulating the window. */
	class Window
	{
	public:
		/* Struct containing window properties. */
		struct WindowSettings
		{
			unsigned int Width;
			unsigned int Height;
			std::wstring Title;
			// TODO: Add window icon support
			// TODO: Add full-screen support
			// TODO: Add V-sync support

			WindowSettings(
				unsigned int width = 1280,
				unsigned int height = 720,
				const std::wstring& title = L"Enterprise")
				: Width(width), Height(height), Title(title) {}
		};

		// Setters
		//virtual void SetWindowSettings(WindowSettings & newSettings); TODO: Add ability to update window settings

		// Getters
		// Gets the width, in pixels, of the game window's viewing area.
		static unsigned int GetWidth() { return m_Instance->m_Settings.Width; };
		// Gets the height, in pixels, of the game window's viewing area.
		static unsigned int GetHeight() { return m_Instance->m_Settings.Height; };

		// Creates the game window.
		static Window* Create(const WindowSettings& attributes = WindowSettings());
		// Destroys the game window.
		static void Destroy() { EP_ASSERT(m_Instance); delete m_Instance; };

		virtual ~Window() {}
	protected:
		static Window* m_Instance; // The singleton instance.
		WindowSettings m_Settings; // The window's current properties.
		Window(const WindowSettings& settings) : m_Settings(settings) {}; // Marked protected for singleton purposes
	};
}
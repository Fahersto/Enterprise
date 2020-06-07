#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

/// The interface to the game window.
class Window
{
public:

	/// Struct describing a game window configuration.
	struct WindowSettings
	{
		/// The width of the view area of the game window.
		unsigned int Width;
		/// The height of the view area of the game window.
		unsigned int Height;
		/// The game window title.
		std::wstring Title;

		// TODO: Add window icon support
		// TODO: Add full-screen support
		// TODO: Add V-sync support

		/// WindowSettings constructor.
		/// @param width The width of the view area of the game window.
		/// @param height The height of the view area of the game window.
		/// @param title The game window title.
		WindowSettings(
			unsigned int width = 1280,
			unsigned int height = 720,
			const std::wstring& title = L"Enterprise")
			: Width(width), Height(height), Title(title) {}
	};


	/// Creates the game window.
	/// @param settings The desired configuration for the new window.
	/// @return A pointer to the Window object.
	static Window* Create(const WindowSettings& settings = WindowSettings());

	/// Destroys the game window.
	static void Destroy() { EP_ASSERT(m_Instance); delete m_Instance; };


	/// Gets the current configuration of the game window.
	/// @return The current configuration of the game window.
	static WindowSettings& GetConfiguration() { return m_Instance->m_Settings; }

	// TODO: Add ability to change window settings from code
	//virtual void SetWindowSettings(WindowSettings & newSettings);


	virtual ~Window() {}
protected:
	/// A pointer to the singleton instance.
	static Window* m_Instance;
	/// This window's current configuration.
	WindowSettings m_Settings;

	/// Default constructor for Window.
	/// @param settings The desired configuration for the new window.
	Window(const WindowSettings& settings) : m_Settings(settings) {};
};

}

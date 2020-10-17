#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	/// The heart of an Enterprise program.  Instantiates and manages all of Enterprise's systems.
	class Application
	{
	public:
		// Steps the game engine.  Returns true until application stops running.  Should only be called in the main function.

		/// Steps the entire Enterprise engine.  Called continuously by the main loop.
		/// @return Boolean indicating whether the main loop should continue.
		/// @note The Run function's return value is used as the condition for the platform-specific main loop.
		bool Run();

		/// Quit the application at the end of the current frame.
		static void Quit();

		/// Sets up the core systems of the application.
		Application();

		/// Cleans up the application prior to termination.
		/// @note Even when the program is terminated due to an exception being thrown, this destructor is called.
		~Application();

	private:
		/// The return value used in @cRun().
		/// @remarks This value is set to false upon a call to @cApplication::Quit().
		static bool _isRunning;

		/// Application's event handler.
		/// @param e The event reference.
		/// @return Whether Application wishes to block the event from propogation.
		static bool OnEvent(Events::Event& e);
	};
}

#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	/// The heart of an Enterprise program.  Instantiates and manages all of 
	/// Enterprise's systems, and steps all engine and game code.
	/// @note Enterprise manages the application lifetime itself: do not try to 
	/// instantiate an Application in game code.
	class Application
	{
	public:
		/// Step the entire Enterprise engine.
		/// @return Boolean indicating whether the main loop should continue.
		/// @remarks This function is called continuously by the main loop. Its 
		/// return value is used as the condition for the loop.
		bool Run();

		/// Quit the application at the end of the current frame.
		static void Quit();

		/// Set up the core systems of the application.
		Application();

		/// Clean up the application prior to program termination.
		/// @note This destructor guaranteed to be called even in the case of 
		/// unhandled exceptions.
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

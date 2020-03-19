#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	/* Application
		The heart of an Enterprise application.  This class instantiates and manages all of Enterprise's systems. */
	class Application
	{
	public:
		// Steps the game engine.  Returns true until application stops running.  Should only be called in the main function.
		bool Run();

		// Quit the application at the end of the current frame.
		static void Quit();

		Application();
		~Application();
	private:
		// Return value of Run().
		static bool _isRunning;
		// The application event handler.  This is a private function to 
		static bool OnEvent(Events::EventPtr e);
	};
}
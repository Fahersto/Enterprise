#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	/* Application
		The heart of the Enterprise application.  This class initializes and operates all of Enterprise's systems. */
	class Application
	{
	public:
		// Step the game engine.  Returns true until application stops running.
		bool Run();

		// Quit the application at the end of the current frame.
		static void Quit();

		Application();
		~Application();
	private:

	};
}
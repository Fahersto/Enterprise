#pragma once

/* Application
	Application is the base class for any Enterprise program.

	Applications are responsible for setting the initial conditions of the game, and managing the
	lifetime of the program.  Each game project will extend Application.  The derived class's constructor
	will be used to set up initial conditions, while the base class will manage the moment-to-moment
	processes of the engine.

	The only thing that Application does not manage is the debug console.  The console is initialized
	in the build platform's main function in Debug builds, and automatically stripped from Release and
	Dist builds.  The Console is the only portable piece of the engine considered more fundamental than
	Application.
*/
#include "Enterprise/Events/CoreEvents.h"

namespace Enterprise
{
	class Application
	{
	public:
		// Steps the entire engine.  Returns true until application stops running.
		bool Run();

		// Call this to quit the application at the end of the current frame.
		static void Quit();

		Application();
		virtual ~Application();

	private:
		// Event Handler
		static bool OnEvent_CoreApp(Event::EventPtr e);

		static Application* m_Instance;
		bool isRunning = true;
	};

	// Creator/getter for client application.  Defined in client.
	extern Application* CreateApplication();
}
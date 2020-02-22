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
		Application(); // Extended in children
		virtual ~Application();

		void SimStep(); // Called at SimRate (fixed rate, configurable per title)
		bool FrameStep(float deltaTime, float simPhase); // Called every frame (variable rate)

		static void Quit();
	private:
		static Application* m_Instance;
		bool isRunning = true;

		static bool OnEvent_CoreApp(Event::EventPtr e);
	};

	// Creator/getter for client application.  Defined in Client.
	Application* CreateApplication();
}
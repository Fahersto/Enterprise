#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	/* Application
		Application is the base class for any Enterprise game.  It manages the program's initial conditions and lifetime.
	*/
	class Application
	{
	public:
		// Steps the entire engine.  Returns true until application stops running.
		bool Run();

		// Calling this quits the application at the end of the current frame.
		static void Quit();

		virtual ~Application();

	private:
		// Event Handler
		static bool OnEvent_CoreApp(Events::EventPtr e);

		static Application* m_Instance;
		bool isRunning = true;
		
	protected:
		Application();
	};

	// Creator/getter for client application.  Defined in client.
	extern Application* CreateApplication();
}
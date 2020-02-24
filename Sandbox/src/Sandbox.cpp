#include <Enterprise.h>
#include "Events/SandboxEvents.h"

Enterprise::Window* gameWindow;

bool OnEvent(Enterprise::Event::EventPtr e)
{
	EP_TRACE(e);
	return false;
}


class SandboxApp : public Enterprise::Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	//TODO: Conditionalize starting conditions between debug and release builds.
	SandboxApp()
	{
		// Temporary: Subscribe to all client events
		using Enterprise::Event::Dispatcher;
		Dispatcher::SubscribeToCategory(Sandbox::Event::CategoryIDs::_All, OnEvent);

		gameWindow = Enterprise::Window::Create();

		EP_ASSERT(1000 == 1);
	}

	//Called at program end
	~SandboxApp()
	{
		delete gameWindow;
	}
};

Enterprise::Application* Enterprise::CreateApplication() { return new SandboxApp(); }
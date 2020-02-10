#include <Enterprise.h>
#include "Events/SandboxEvents.h"

//using namespace Sandbox;
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
	SandboxApp()
	{
		//TODO: Conditionalize starting conditions between debug and release builds.

		// Temporary: Subscribe to all client events
		Enterprise::Event::Dispatcher::SubscribeToCategory(Sandbox::Event::CategoryIDs::_All, OnEvent);

		gameWindow = Enterprise::Window::Create();
	}

	//Called at program end
	~SandboxApp()
	{
		delete gameWindow;
	}
};

Enterprise::Application* Enterprise::CreateApplication() { return new SandboxApp(); }
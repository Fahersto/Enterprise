#include <Enterprise.h>
#include "Events/SandboxEvents.h"

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
		// Temporary: Subscribe to all client events
		using Enterprise::Event::Dispatcher;
		Dispatcher::SubscribeToCategory(Sandbox::Event::CategoryIDs::_All, OnEvent);

		Enterprise::Window::Create();
	}

	//Called at program end
	~SandboxApp()
	{
		Enterprise::Window::Destroy();
	}
};

Enterprise::Application* Enterprise::CreateApplication() { return new SandboxApp(); }
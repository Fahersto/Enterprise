#include <Enterprise.h>
#include "Events/OLD/SandboxEvents.h"
#include "Enterprise/Events/Events.h"

bool OnEvent(Enterprise::Event::EventPtr e)
{
	// Temporary: trace all events.
	EP_TRACE(e);
	return false;
}


class SandboxApp : public Enterprise::Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	SandboxApp()
	{
		// Temporary: Subscribe to all events except mouse move.
		using Enterprise::Event::Dispatcher;
		Dispatcher::SubscribeToCategory(Enterprise::Event::CategoryIDs::_All, OnEvent);
		Dispatcher::UnsubscribeFromType(Enterprise::Event::TypeIDs::MousePosition, OnEvent);
		Dispatcher::SubscribeToCategory(Sandbox::Event::CategoryIDs::_All, OnEvent);

		Enterprise::Window::Create(); // TODO: Assert when no window is created.

		EP_EVENTCATEGORY(Poo);
		EP_EVENTCATEGORY(Poop);
		EP_EVENTCATEGORY(Pooop);
		EP_EVENTCATEGORY(Poooop);

		EP_EVENTTYPE(Test, Poo);
		EP_EVENTTYPE(Test2, Poop);
		EP_EVENTTYPE(Test3, Poo | Poop);
		EP_EVENTTYPE(Test4, Poooop | Poop);
	}

	//Called at program end
	~SandboxApp()
	{
		Enterprise::Window::Destroy();
	}
};

// Set SandboxApp as the Application to use during runtime
Enterprise::Application* Enterprise::CreateApplication() { return new SandboxApp(); }
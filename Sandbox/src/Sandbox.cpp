#include <Enterprise.h>
#include "Enterprise/Events/Events.h"
#include "Enterprise/Events/Dispatcher.h"

//#include "Events\SandboxEvents.h"

using namespace Enterprise;

bool OnEvent(std::shared_ptr<Event> e)
{
	EP_TRACE(e);
	return false;
}

class Sandbox : public Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	Sandbox()
	{
		//TODO: Conditionalize starting conditions between debug and release builds.

		EP_INFO("Sandbox instantiated!");

		// Subscribe to all event types
		//// TODO: make "all" category
		//for (int i = 0; i < (size_t)EventType::NumOfTypes; i++)
		//{
		//	Dispatcher::SubscribeToType(EventType(i), OnEvent);
		//}

		Dispatcher::SubscribeToCategory(EventCategory::Application, OnEvent);

		EP_QUICKEVENT(Events::E_WindowMove, 100, 100);
		EP_QUICKEVENT(Events::E_MouseButtonDown, 1);

	}

	//Called at program end
	~Sandbox()
	{
		EP_INFO("Sandbox destroyed.");
	}
};

Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new Sandbox();
}
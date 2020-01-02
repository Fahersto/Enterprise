#include <Enterprise.h>
#include "Enterprise/Events/CoreEvents.h"
#include "Events\SandboxEvents.h"
#include "Enterprise/Events/Dispatcher.h"

using namespace Enterprise;

bool OnEvent(std::shared_ptr<Event::Event> e)
{
	EP_TRACE(e);
	return false;
}

class SandboxApp : public Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	SandboxApp()
	{
		//TODO: Conditionalize starting conditions between debug and release builds.

		EP_INFO("SandboxApp instantiated!");

		Event::Dispatcher::SubscribeToCategory(Event::CategoryIDs::Application, OnEvent);

		EP_QUICKEVENT(Event::WindowMove, 100, 100);
		EP_QUICKEVENT(Event::MouseButtonDown, 1);
	
		Sandbox::Event::NewGameMode e(10);
		EP_TRACE(e);
	}

	//Called at program end
	~SandboxApp()
	{
		EP_INFO("SandboxApp destroyed.");
	}
};

Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new SandboxApp();
}
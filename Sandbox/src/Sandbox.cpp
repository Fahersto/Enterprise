#include <Enterprise.h>

#include "Events\SandboxEvents.h"
using namespace Sandbox;

bool OnEvent(EP_EVENTPTR e)
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

		// Subscribe to all events (except for mouse position events).
		Enterprise::Dispatcher::SubscribeToCategory(Event::CategoryIDs::_All, OnEvent);
		Enterprise::Dispatcher::SubscribeToCategory(Enterprise::Event::CategoryIDs::_All, OnEvent);
		Enterprise::Dispatcher::UnsubscribeFromType(Enterprise::Event::TypeIDs::MousePosition, OnEvent);
	}

	//Called at program end
	~SandboxApp()
	{
	}
};

Enterprise::Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new SandboxApp();
}
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

		EP_INFO("SandboxApp instantiated!");

		Enterprise::Dispatcher::SubscribeToCategory(Event::CategoryIDs::PlayerInfo, OnEvent);
		Enterprise::Dispatcher::SubscribeToCategory(Enterprise::Event::CategoryIDs::Input, OnEvent);

		EP_QUICKEVENT(Enterprise::Event::KeyChar, 'a');
		EP_QUICKEVENT(Event::PlayerPosition, 10.1, 5.2, 1.3);
	}

	//Called at program end
	~SandboxApp()
	{
		EP_INFO("SandboxApp destroyed.");
	}
};

Enterprise::Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new SandboxApp();
}
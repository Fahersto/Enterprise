#include <Enterprise.h>
#include "Events/SandboxEvents.h"

//using namespace Sandbox;
Enterprise::Window* gameWindow;

bool OnEvent(Enterprise::Event::EventPtr e)
{
	return false;
}


class SandboxApp : public Enterprise::Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	SandboxApp()
	{
		//TODO: Conditionalize starting conditions between debug and release builds.

		//// Subscribe to all events (except for mouse position events).
		//Enterprise::Event::Dispatcher::SubscribeToCategory(Sandbox::Event::CategoryIDs::_All, OnEvent);
		//Enterprise::Event::Dispatcher::SubscribeToCategory(Enterprise::Event::CategoryIDs::_All, OnEvent);
		//Enterprise::Event::Dispatcher::UnsubscribeFromType(Enterprise::Event::TypeIDs::MousePosition, OnEvent);

		gameWindow = Enterprise::Window::Create();
	}

	//Called at program end
	~SandboxApp()
	{
		delete gameWindow;
	}
};

Enterprise::Application* Enterprise::CreateApplication() { return new SandboxApp(); }
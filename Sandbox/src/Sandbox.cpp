#include <Enterprise.h>

#include "Enterprise/Input/InputEvents.h"
#include "Enterprise/Application/ApplicationEvents.h"

bool OnEvent(Enterprise::Events::EventPtr e)
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
		// Temporary: subscribe to all event types (except mouse position).
		using Enterprise::Events;
		Events::SubscribeToType(EventTypes::KeyChar, OnEvent);
		Events::SubscribeToType(EventTypes::KeyDown, OnEvent);
		Events::SubscribeToType(EventTypes::KeyUp, OnEvent);
		
		Events::SubscribeToType(EventTypes::MouseButtonDown, OnEvent);
		Events::SubscribeToType(EventTypes::MouseButtonUp, OnEvent);
		Events::SubscribeToType(EventTypes::MouseDelta, OnEvent);
		//Events::SubscribeToType(EventTypes::MousePosition, OnEvent);
		Events::SubscribeToType(EventTypes::MouseScroll, OnEvent);
		
		Events::SubscribeToType(EventTypes::WindowClose, OnEvent);
		Events::SubscribeToType(EventTypes::WindowFocus, OnEvent);
		Events::SubscribeToType(EventTypes::WindowLostFocus, OnEvent);
		Events::SubscribeToType(EventTypes::WindowMove, OnEvent);
		Events::SubscribeToType(EventTypes::WindowResize, OnEvent);

		Enterprise::Window::Create(); // TODO: Assert when no window is created.
	}

	//Called at program end
	~SandboxApp()
	{
		Enterprise::Window::Destroy();
	}
};

// Set SandboxApp as the Application to use during runtime
Enterprise::Application* Enterprise::CreateApplication() { return new SandboxApp(); }
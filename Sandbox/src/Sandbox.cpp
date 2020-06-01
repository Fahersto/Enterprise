#include <Enterprise.h>
#include <Enterprise/Application/Game.h>

#include <Enterprise/Input/InputEvents.h>
#include <Enterprise/Application/ApplicationEvents.h>


bool OnEvent(Enterprise::Events::Event& e)
{
	// Temporary: trace all events.
    EP_TRACE(e.ToString());
	return false;
}

void Enterprise::Game::Init()
{
	// Temporary: subscribe to all event types (except mouse position).
	Events::SubscribeToType(EventTypes::KeyChar, OnEvent);
	Events::SubscribeToType(EventTypes::KeyDown, OnEvent);
	Events::SubscribeToType(EventTypes::KeyUp, OnEvent);

	Events::SubscribeToType(EventTypes::MouseButtonDown, OnEvent);
	Events::SubscribeToType(EventTypes::MouseButtonUp, OnEvent);
	Events::SubscribeToType(EventTypes::MouseDelta, OnEvent);
	//Events::SubscribeToType(EventTypes::MousePosition, OnEvent);
	Events::SubscribeToType(EventTypes::MouseScroll, OnEvent);

	Events::SubscribeToType(EventTypes::WindowFocus, OnEvent);
	Events::SubscribeToType(EventTypes::WindowLostFocus, OnEvent);
	Events::SubscribeToType(EventTypes::WindowMove, OnEvent);
	Events::SubscribeToType(EventTypes::WindowResize, OnEvent);

	Window::Create(); // TODO: Assert when no window is created.
    
    Events::Event e = EventTypes::KeyChar;
    Events::Dispatch(e);
    
    Events::Dispatch(EventTypes::KeyChar, std::tuple<char, float, int, const char*>('a', 1.5, 10, "poo"));
}

void Enterprise::Game::Cleanup()
{
	Window::Destroy();
}

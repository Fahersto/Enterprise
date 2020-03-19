#include <Enterprise.h>
using Enterprise::Game;

#include "Enterprise/Input/InputEvents.h"
#include "Enterprise/Application/ApplicationEvents.h"

bool OnEvent(Enterprise::Events::EventPtr e)
{
	// Temporary: trace all events.
	EP_TRACE(e);
	return false;
}

void Game::Init()
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

void Game::Cleanup()
{
	Window::Destroy();
}
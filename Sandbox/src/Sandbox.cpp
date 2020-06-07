#include <Enterprise.h>

using Enterprise::Game;
using Enterprise::Window;

using Enterprise::Events;
#include <Enterprise/Input/InputEvents.h>
#include <Enterprise/Application/ApplicationEvents.h>


bool onKeyCharEvent(Events::Event& e)
{
	// Example: Unpack a single variable
	char letter = Events::Unpack<char>(e);

	EP_TRACE("KeyChar event handled!  Value: {}", letter);
	return true; // Marks this event as handled, blocking it.
}

bool onMousePositionEvent(Events::Event& e)
{
	// Example: Unpack a tuple using structured bindings
	auto [x, y] = Events::Unpack<std::pair<int, int>>(e);

	EP_TRACE("MousePosition event handled!  Values: x = {}, y = {}", x, y);
	return false; // Allows event to continue propogating down the callback list.
}

bool onWindowEvents(Events::Event& e)
{
	// Example: Directly log events
	EP_DEBUG("Generic event handled: {}", e.DebugString());

	return false; // Try changing this to true, then closing the window!
}


void Game::Init()
{
	Events::SubscribeToCategory(EventCategories::Window, onWindowEvents);
	Events::SubscribeToType(EventTypes::KeyChar, onKeyCharEvent);
	Events::SubscribeToType(EventTypes::MousePosition, onMousePositionEvent);

	Window::Create(Window::WindowSettings(500, 500, L"Test Window Title"));
}

void Game::Cleanup()
{
	Window::Destroy();
}

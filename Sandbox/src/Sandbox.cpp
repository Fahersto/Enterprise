#include <Enterprise.h>
#include <Enterprise/Application/Game.h>
#include <Enterprise/Input/InputEvents.h>
#include <Enterprise/Application/ApplicationEvents.h>

using Enterprise::Events;

bool OnEvent(Events::Event& e)
{
	// Example: Unpack a single variable
	if (e.Type() == EventTypes::KeyChar)
	{
		char letter = Events::Unpack<char>(e);
		EP_DEBUG("This is a letter: {}", letter);
	}

	// Example: Unpack a tuple using structured bindings (C++17)
	else if (e.Type() == EventTypes::MousePosition)
	{
		auto [x, y] = Events::Unpack<std::pair<int, int>>(e);
		EP_DEBUG("Unpacked MousePosition: x = {}, y = {}", x, y);
	}

	// Example: log event.  Note that all logging code is stripped from non-debug configurations.
	EP_TRACE(e.DebugString());

	return false;
}


void Enterprise::Game::Init()
{
	Events::SubscribeToCategory(EventCategories::Window, OnEvent);
	Events::SubscribeToType(EventTypes::KeyChar, OnEvent);
	Events::SubscribeToType(EventTypes::MousePosition, OnEvent);

	Window::Create(Window::WindowSettings(500, 500, L"Test Window Title"));
}

void Enterprise::Game::Cleanup()
{
	Window::Destroy();
}

#include <Enterprise.h>
#include <Enterprise/Application/Game.h>

#include <Enterprise/Input/InputEvents.h>
#include <Enterprise/Application/ApplicationEvents.h>


bool OnEvent(Enterprise::Events::Event& e)
{
	// Temporary: trace all events.
    EP_TRACE(e.DebugString());
	return false;
}

void Enterprise::Game::Init()
{
	Window::Create(); // TODO: Assert when no window is created.
}

void Enterprise::Game::Cleanup()
{
	Window::Destroy();
}

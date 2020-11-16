#include <Enterprise.h>

using Enterprise::Game;

void Game::Init()
{
	Window::Create(Window::WindowSettings(500, 500, L"Test Window Title"));
}

void Game::Cleanup()
{
	Window::Destroy();
}

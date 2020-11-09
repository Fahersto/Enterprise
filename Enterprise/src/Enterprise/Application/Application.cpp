#include "EP_PCH.h"
#include "Core.h"

// Application stuff
#include "Application.h"
#include "Game.h"
#include "Window.h"
#include "Enterprise/Application/ApplicationEvents.h"

// Systems
#include "Enterprise/Time/Time.h"
#include "Enterprise/Input/Input.h"

namespace Enterprise 
{
	bool Application::_isRunning = true;

	Application::Application()
	{
		EP_ASSERT_NOREENTRY(); // Disallow creating a second Application.

		// Create Console
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Init();
		#endif

		// Initialize Systems
		Time::Init();
		// File::Init();
		// Network::Init();
		Input::Init();
		// Graphics::Init();
		// Audio::Init();
		// ECS::Init();
		// StateStack::Init();

		// Event subscriptions
		Events::SubscribeToType(EventTypes::WindowClose, OnEvent);
        Events::SubscribeToType(EventTypes::QuitRequested, OnEvent);

		Game::Init();

		// TODO: Assert if no window is created.

		//EP_ASSERT(false);
	}

	Application::~Application()
	{
		Game::Cleanup();

		// Clean up the console
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Cleanup();
		#endif
	}

	// Main loop
	bool Application::Run()
	{
		// Physics frame
		while (Time::PhysFrame())
		{
			// ...
		}

		// Frame
		Time::FrameStart();
		Input::Update();
		// ...
		Time::FrameEnd();

		// Back to main function
		return _isRunning;
	}

	void Application::Quit() { _isRunning = false; }

    bool Application::OnEvent(Events::Event& e)
    {
        if (e.Type() == EventTypes::WindowClose)
            // By default, closing the window is treated as a request to quit.
            Enterprise::Events::Dispatch(EventTypes::QuitRequested);
        else if (e.Type() == EventTypes::QuitRequested)
            // By default, selecting Quit from the macOS dock or app menu quits the program.
			Enterprise::Application::Quit();
		return true;
	}
}

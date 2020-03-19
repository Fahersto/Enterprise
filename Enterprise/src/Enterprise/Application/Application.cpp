#include "EP_PCH.h"
#include "Core.h"

// Application stuff
#include "Application.h"
#include "Game.h"
#include "Window.h"
#include "Enterprise/Application/ApplicationEvents.h"

// Systems
#include "Enterprise/Time/Time.h"

bool _isRunning = true;
Enterprise::Game _game;

namespace Enterprise 
{
	bool OnEvent(Events::EventPtr e)
	{
		// This behavior can be overridden by handling WindowClose events elsewhere.
		if (e->GetType() == EventTypes::WindowClose)
			Application::Quit();
		return true;
	}

	Application::Application()
	{
		// Create Console
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Init();
		#endif

		_game.Init();

		// Initialize Systems
		Time::Init();
		// File::Init();
		// Network::Init();
		// Input::Init();
		// Graphics::Init();
		// Audio::Init();
		// ECS::Init();
		// StateStack::Init();

		// Event subscriptions
		Events::SubscribeToType(EventTypes::WindowClose, OnEvent);
	}

	Application::~Application()
	{
		_game.Cleanup();

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
		// ...
		Time::FrameEnd();

		// Back to main function
		return _isRunning;
	}

	// Quit function
	void Application::Quit() { _isRunning = false; }
}
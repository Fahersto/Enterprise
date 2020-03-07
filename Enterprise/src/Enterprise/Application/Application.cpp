#include "EP_PCH.h"
#include "Core.h"

// Application stuff
#include "Application.h"
#include "Window.h"

// Systems
#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Time/Time.h"

namespace Enterprise 
{
	Application* Application::m_Instance = nullptr;

	// Constructor
	Application::Application()
	{
		// Singleton handling
		EP_ASSERT(!m_Instance); // Error: Applications are singletons.
		m_Instance = this;

		// Create Console
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Init();
		#endif

		// Initialize Systems
		Event::Dispatcher::Init();
		Time::Init();
		// File::Init();
		// Network::Init();
		// Input::Init();
		// Graphics::Init();
		// Audio::Init();
		// ECS::Init();
		// StateStack::Init();

		// Event subscriptions
		//Event::Dispatcher::SubscribeToType(Event::TypeIDs::QuitApplication, OnEvent_CoreApp);
		
		// Temporary: Subscribe to all core events except for mouse events
		Event::Dispatcher::SubscribeToCategory(Event::CategoryIDs::_All, OnEvent_CoreApp);
		Event::Dispatcher::UnsubscribeFromType(Event::TypeIDs::MousePosition, OnEvent_CoreApp);
	}

	// Destructor
	Application::~Application()
	{
		Event::Dispatcher::Cleanup();

		// Clean up the console
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Cleanup();
		#endif
	}

	// Run loop
	bool Application::Run()
	{
		while (Time::PhysFrame())
		{
			// Physics frame here
		}

		Time::FrameStart();

		// General frame here

		Event::Dispatcher::Update();

		Time::FrameEnd();

		return isRunning;
	}

	// Event handler
	bool Application::OnEvent_CoreApp(Event::EventPtr e)
	{
		// Temporary: Log all events.
		EP_TRACE(e);

		// Default behavior: if the client doesn't handle WindowClose events, they quit the application.
		if (e->GetTypeID() == Event::TypeIDs::WindowClose)
			Quit();
		return true;
	}

	// Quit function
	void Application::Quit() { m_Instance->isRunning = false; }
}
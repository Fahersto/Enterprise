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
		Event::Dispatcher::SubscribeToType(Event::TypeIDs::WindowClose, OnEvent_CoreApp);
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
		// Physics frame
		while (Time::PhysFrame())
		{
			// ...
		}

		// Frame
		Time::FrameStart();
		Event::Dispatcher::Update();
		// ...
		Time::FrameEnd();

		// Back to main function
		return isRunning;
	}

	// Event handler
	bool Application::OnEvent_CoreApp(Event::EventPtr e)
	{
		// By default, WindowClose events quit the application.
		if (e->GetTypeID() == Event::TypeIDs::WindowClose)
			Quit();
		return true;
	}

	// Quit function
	void Application::Quit() { m_Instance->isRunning = false; }
}
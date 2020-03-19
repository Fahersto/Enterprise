#include "EP_PCH.h"
#include "Core.h"

// Application stuff
#include "Application.h"
#include "Window.h"
#include "Enterprise/Application/ApplicationEvents.h"

// Systems
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
		Time::Init();
		// File::Init();
		// Network::Init();
		// Input::Init();
		// Graphics::Init();
		// Audio::Init();
		// ECS::Init();
		// StateStack::Init();

		// Event subscriptions
		Events::SubscribeToType(EventTypes::WindowClose, OnEvent_CoreApp);
	}

	// Destructor
	Application::~Application()
	{
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
		// ...
		Time::FrameEnd();

		// Back to main function
		return isRunning;
	}

	// Event handler
	bool Application::OnEvent_CoreApp(Events::EventPtr e)
	{
		// This behavior can be overridden by handling WindowClose events elsewhere.
		if (e->GetType() == EventTypes::WindowClose)
			Quit();
		return true;
	}

	// Quit function
	void Application::Quit() { m_Instance->isRunning = false; }
}
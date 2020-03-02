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
	Application* Application::m_Instance = nullptr; // Singleton instance

	// Constructor
	Application::Application()
	{
		// Singleton handling
		if (!m_Instance)
			m_Instance = this;
		else
			EP_ERROR("Application constructor called twice.  Application is intended to be a singleton.");

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

		// Temporary: Subscribe to all core events except for mouse events
		Event::Dispatcher::SubscribeToCategory(Event::CategoryIDs::_All, OnEvent_CoreApp);
		Event::Dispatcher::UnsubscribeFromType(Event::TypeIDs::MousePosition, OnEvent_CoreApp);
		//Event::Dispatcher::SubscribeToType(Event::TypeIDs::QuitApplication, OnEvent_CoreApp); //Uncomment this when removing above two lines
	}

	// Destructor
	Application::~Application()
	{
		Event::Dispatcher::Cleanup();
	}

	// Run loop
	bool Application::Run()
	{
		while (Time::SimStep())
		{
			// SimStep here
		}

		Time::FrameStep_begin();

		// FrameStep here
		Event::Dispatcher::Update();

		Time::FrameStep_end();

		return isRunning;
	}

	// Event handler
	bool Application::OnEvent_CoreApp(Event::EventPtr e)
	{
		if (e->GetTypeID() == Event::TypeIDs::WindowClose)
			Quit();
		return true;
	}

	// Quit function
	void Application::Quit() { m_Instance->isRunning = false; }
}
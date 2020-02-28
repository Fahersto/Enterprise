#include "EP_PCH.h"
#include "Application.h"

#include "Console.h"

#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Time/Time.h"

#include "Window.h"

namespace Enterprise {

	Application* Application::m_Instance = nullptr;

	// EVENT HANDLER ------------------------------------------------------------------------------
	bool Application::OnEvent_CoreApp(Event::EventPtr e)
	{
		EP_TRACE(e);
		if (e->GetTypeID() == Event::TypeIDs::WindowClose)
			Quit();
		return false;
	}

	void Application::Quit()
	{
		m_Instance->isRunning = false;
	}
	
	// CONSTRUCTOR DESTRUCTOR ---------------------------------------------------------------------
	Application::Application()
	{
		if (!m_Instance)
			m_Instance = this;
		else
			EP_ERROR("Application constructor called twice.  Application is intended to be a singleton.");

		EP_TRACE("Application created");
		Event::Dispatcher::Init();

		// Temporary: Subscribe to all core events except for mouse events
		Event::Dispatcher::SubscribeToCategory(Event::CategoryIDs::_All, OnEvent_CoreApp);
		Event::Dispatcher::UnsubscribeFromType(Event::TypeIDs::MousePosition, OnEvent_CoreApp);
	}
	Application::~Application()
	{
		Event::Dispatcher::Cleanup();
		EP_TRACE("Application destroyed");
	}

	// CORE CALLS ---------------------------------------------------------------------------------
	void Application::SimStep()
	{
		//Time::m_deltaTime = ...?
		Time::m_simPhase = 1.0f; // or 0.0f?

		//Propogate SimStep here
	}

	bool Application::FrameStep(float deltaTime, float simPhase)
	{
		// Set values in Time system
		Time::m_deltaTime = deltaTime;
		Time::m_simPhase = simPhase;

		Event::Dispatcher::Update();

		//Propogate Update, PostUpdate, and Draw through the engine

		return isRunning;
	}
}
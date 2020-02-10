#include "EP_PCH.h"
#include "Application.h"

#include "Console.h"
#include "Enterprise/Events/Dispatcher.h"

#include "Window.h"

namespace Enterprise {
	
	// EVENT HANDLER ------------------------------------------------------------------------------
	bool Application::OnEvent_CoreApp(Event::EventPtr e)
	{
		EP_TRACE(e);
		return false;
	}
	
	// CONSTRUCTOR DESTRUCTOR ---------------------------------------------------------------------
	Application::Application()
	{
		EP_TRACE("Application created!");
		Event::Dispatcher::Init();
		EP_TRACE("Dispatcher.Init() called");

		// Temporary: Subscribe to all core events except for mouse events
		Event::Dispatcher::SubscribeToCategory(Event::CategoryIDs::_All, OnEvent_CoreApp);
		Event::Dispatcher::UnsubscribeFromType(Event::TypeIDs::MousePosition, OnEvent_CoreApp);
	}
	Application::~Application()
	{
		Event::Dispatcher::Cleanup();
		EP_TRACE("Application destroyed.");
	}

	// CORE CALLS ---------------------------------------------------------------------------------
	void Application::SimStep(float deltaTime)
	{
	}

	void Application::Update(float deltaTime)
	{
		Event::Dispatcher::Update();
	}
	void Application::PostUpdate(float deltaTime)
	{
	}
	void Application::Draw(float simInterp)
	{
	}
}
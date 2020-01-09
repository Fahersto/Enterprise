#include "EP_PCH.h"
#include "Application.h"

#include "Console.h"
#include "Enterprise/Events/Dispatcher.h"

#include "Window.h"

namespace Enterprise {
	Application::Application()
	{
		EP_TRACE("Application created!");
		Event::Dispatcher::Init(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		EP_TRACE("Dispatcher.Init() called");
	}

	void Application::Tick()
	{
	}

	void Application::Update()
	{
		Event::Dispatcher::Update();
	}

	void Application::Draw()
	{
	}

	Application::~Application()
	{
		Event::Dispatcher::Cleanup();
		EP_TRACE("Application destroyed.");
	}

	bool Application::OnEvent(Event::EventPtr e)
	{
		return false;
	}
}
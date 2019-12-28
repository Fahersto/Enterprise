#include "EP_PCH.h"
#include "Application.h"

#include "Console.h"

#include "Enterprise/Event/Event.h"
#include "Enterprise/Event/Dispatcher.h"

bool OnEvent_BaseApplication(std::shared_ptr<Enterprise::Event> e)
{
	EP_TRACE("Application::OnEvent called: {}", *e);
	return true;
}

namespace Enterprise {
	Application::Application()
	{
		Dispatcher::Init();
	}

	void Application::Tick()
	{
	}

	void Application::Update()
	{
		Dispatcher::Update();
	}

	void Application::Draw()
	{
	}

	Application::~Application()
	{
	}
}
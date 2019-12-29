#include <Enterprise.h>
#include "Enterprise/Event/Dispatcher.h"
#include "Enterprise/Event/MouseEvent.h"
#include "Enterprise/Event/KeyEvent.h"

using namespace Enterprise;

bool OnEvent(std::shared_ptr<Event> e)
{
	EP_TRACE("OnEvent called for {}!", e);
	return false;
}

class Sandbox : public Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	Sandbox()
	{
		//TODO: Conditionalize this between debug and release builds.
		EP_INFO("Sandbox instantiated!");

		//Dispatcher::SubscribeToType(EventType::MouseButtonDown, OnEvent);
		//Dispatcher::SubscribeToType(EventType::MouseScroll, OnEvent);
		//Dispatcher::SubscribeToType(EventType::MouseButtonUp, OnEvent);
		//Dispatcher::SubscribeToType(EventType::KeyDown, OnEvent);

		Dispatcher::SubscribeToCategory(EventCategory::Keyboard, OnEvent);

		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseButtonDown>(1));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseScroll>(2, 2));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseButtonUp>(3));
		Dispatcher::BroadcastEvent(std::make_shared<Event_KeyDown>(4));

		//EP_TRACE("This is a Trace");
		//EP_DEBUG("This is a Debug");
		//EP_INFO("This is an Info");
		//EP_WARN("This is a Warning");
		//EP_ERROR("This is an Error");
		//EP_FATAL("This is a Fatal error");

	}

	//Called at program end
	~Sandbox()
	{
		EP_INFO("Sandbox destroyed.");
	}
};

//EP_STARTSTATE(Sandbox);

Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new Sandbox();
}
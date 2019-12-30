#include <Enterprise.h>
#include "Enterprise/Event/Event.h"
#include "Enterprise/Event/Dispatcher.h"
#include "Enterprise/Event/EventClasses.h"

using namespace Enterprise;

bool OnEvent(std::shared_ptr<Event> e)
{
	EP_TRACE(e);
	return false;
}

class Sandbox : public Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	Sandbox()
	{
		//TODO: Conditionalize starting conditions between debug and release builds.

		EP_INFO("Sandbox instantiated!");

		// Subscribe to all event types
		// TODO: make "all" category
		for (int i = 0; i < (size_t)EventType::NumOfTypes; i++)
		{
			Dispatcher::SubscribeToType(EventType(i), OnEvent);
		}

		// TODO: Add Dispatcher option to log all events (verbose mode?)

		// Test each kind of event
		Dispatcher::BroadcastEvent(std::make_shared<Event_WindowClose>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_WindowMove>(10, 10));
		Dispatcher::BroadcastEvent(std::make_shared<Event_WindowResize>(11, 11));
		Dispatcher::BroadcastEvent(std::make_shared<Event_WindowFocus>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_WindowLostFocus>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_KeyDown>(5));
		Dispatcher::BroadcastEvent(std::make_shared<Event_KeyUp>(5));
		Dispatcher::BroadcastEvent(std::make_shared<Event_KeyChar>(5));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseButtonDown>(5));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseButtonUp>(5));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseDelta>(1, 2));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MouseScroll>(1, 2));
		Dispatcher::BroadcastEvent(std::make_shared<Event_MousePosition>(1, 2));
		Dispatcher::BroadcastEvent(std::make_shared<Event_FileOpenRequest>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_FileOpenComplete>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_FileSaveRequest>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_FileSaveComplete>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_PlayAudioAsset>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_PauseAudioAsset>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_StepAudioAsset>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_AudioParameter>());
		Dispatcher::BroadcastEvent(std::make_shared<Event_StopAudioAsset>());

		// Test all levels of log messages
		EP_TRACE("This is a Trace");
		EP_DEBUG("This is a Debug");
		EP_INFO("This is an Info");
		EP_WARN("This is a Warning");
		EP_ERROR("This is an Error");
		EP_FATAL("This is a Fatal error");
	}

	//Called at program end
	~Sandbox()
	{
		EP_INFO("Sandbox destroyed.");
	}
};

Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new Sandbox();
}
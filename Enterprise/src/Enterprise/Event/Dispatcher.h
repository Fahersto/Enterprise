#pragma once

#include "Event.h"



namespace Enterprise 
{
	class Dispatcher
	{
	public:
		static void Init();

		inline static void BroadcastEvent(std::shared_ptr<Event> event) { eventBuffer.emplace_back(event); }

		// Callbacks take the form bool OnEvent(std::shared_ptr<Event> e).
		inline static void SubscribeToType(EventType type, std::function<bool(std::shared_ptr<Event>)> callback) { 
			callbackLists[(int)type].emplace_back(callback); 
		}

		static void SubscribeToCategory(int category, std::function<bool(std::shared_ptr<Event>)> callback);

		static void Update(); //Dispatches all buffered events

	private:
		// This buffer fills with events between Updates.  At the end of each Update, the events are 
		// dispatched and the buffer is cleared.
		static std::vector<std::shared_ptr<Event>> eventBuffer;

		// This is an array of callback buffers.  Each event type has a vector here, at the array index equal to its EventType.
		static std::vector<std::function<bool(std::shared_ptr<Event>)>> callbackLists[(int)EventType::NumOfTypes];
	};
}
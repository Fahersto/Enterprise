#pragma once
#include "BaseEvent.h"

namespace Enterprise 
{
	/* Dispatcher
		The Dispatcher is Enterprise's event handling system.

		Broadcasted Events are buffered and dispatched to its subscribers, in order of priority, at the 
		start of each Update().  SubscribeToType() allows you to register a callback function for specific
		Event types, and SubscribeToCategory() allows you to subscribe to several at once.
	*/
	class Dispatcher
	{
	public:
		// Add a callback to the callback buffer for the selected EventType.
		// Callbacks take the form bool OnEvent(std::shared_ptr<Event> e).
		inline static void SubscribeToType(EventType type, std::function<bool(std::shared_ptr<Event>)> callback) 
		{ 
			callbackLists[(int)type].emplace_back(callback); 
		}

		// Add a callback to the callback buffers for all EventTypes associated with the selected EventCategory.
		// Callbacks take the form bool OnEvent(std::shared_ptr<Event> e).
		static void SubscribeToCategory(EventCategory category, std::function<bool(std::shared_ptr<Event>)> callback)
		{
			for (auto eventTypeIterator = EventCategoryMatrix.at((size_t)category).begin();
				eventTypeIterator != EventCategoryMatrix.at((size_t)category).end();
				++eventTypeIterator)
			{
				callbackLists[size_t(*eventTypeIterator)].emplace_back(callback);
				//TODO: add a warning if a subscriber is already subscribed to this Event type.
			}
		}

		static void Init(); // Reserves vector sizes, populates EventCategoryMatrix
		static void Update(); // Dispatches all buffered events

		// Buffer an event.  It will be dispatched to the appropriate subscribers at the start of the next Update().
		inline static void BroadcastEvent(std::shared_ptr<Event> event) { eventBuffer.emplace_back(event); }
	private:
		// This buffer fills with events between Updates.  At the end of each Update, the events are 
		// dispatched and the buffer is cleared.
		static std::vector<std::shared_ptr<Event>> eventBuffer;

		// This is an array of callback buffers.  Each event type has a vector here, at the array index equal to its EventType.
		static std::vector<std::function<bool(std::shared_ptr<Event>)>> callbackLists[(int)EventType::NumOfTypes];
		
		// Map of which categories have which types in them. Dim 1: Category, Dim 2: Categories
		static std::array<std::vector<EventType>, size_t(EventCategory::NumOfCategories)> EventCategoryMatrix;
	};

	// Express create and disptach a new Event.
	#define EP_QUICKEVENT(eType, ...) Dispatcher::BroadcastEvent(CreateEvent<eType>(__VA_ARGS__))
}
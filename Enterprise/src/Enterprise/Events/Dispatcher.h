#pragma once
#include "BaseEvent.h"
#include "CoreEvents.h" // TODO: eliminate this dependency

	/* Dispatcher.h
		The Dispatcher is Enterprise's event handling system.

		If you want to receive a broadcasted Event, you can register a callback here.  Events that are broadcasted are dispatched
		in order of priority.

		- Callbacks take the form bool OnEvent(std::shared_ptr<Event> e).
		- If a callback returns true, then the event is blocked from layers farther down the callback stack.
	*/

namespace Enterprise::Event
{
	class Dispatcher
	{
	public:
		// Subscription functions
		inline static void SubscribeToType(unsigned int typeID, std::function<bool(std::shared_ptr<Event>)> callback) {
			callbackLists[typeID].emplace_back(callback);
		}
		static void SubscribeToCategory(unsigned int categoryID, std::function<bool(std::shared_ptr<Event>)> callback);

		// Dispatch function
		inline static void Broadcast(std::shared_ptr<Event> event) { eventBuffer.emplace_back(event); }

		// Core Calls
		static void Init();
		static void Update();
	private:
		// Event buffer (all Event types, in broadcast order)
		static std::vector<std::shared_ptr<Event>> eventBuffer;

		// Callback buffer array (Dim 1: TypeID, Dim 2: Callbacks in order of registration)
		static std::vector<std::function<bool(std::shared_ptr<Event>)>> callbackLists[TypeIDs::NumOfCoreTypes];
		
		// Category map (Dim 1: Category, Dim 2: Categories)
		static std::array<std::vector<unsigned int>, CategoryIDs::NumOfCoreCategories> EventCategoryMatrix;
	};

	// Express create and disptach a new Event.
	#define EP_QUICKEVENT(eType, ...) Enterprise::Event::Dispatcher::Broadcast(Enterprise::Event::CreateEPEvent<eType>(__VA_ARGS__))
}
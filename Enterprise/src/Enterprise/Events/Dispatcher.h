#pragma once
#include "EP_PCH.h"
#include "BaseEvent.h"

/* Dispatcher.h
	The Dispatcher is Enterprise's event handling system.

	If you want to receive a broadcasted Event, you can register a callback here.  Events that are broadcasted are dispatched
	in order of priority.

	- Callbacks take the form bool OnEvent(std::shared_ptr<Event> e).
	- If a callback returns true, then the event it was handling is blocked from the rest of the callback stack.
*/

namespace Enterprise::Event
{
	class Dispatcher
	{
	public:
		// Subscription functions
		inline static void SubscribeToType(unsigned int typeID, EventCallbackPtr callback) { callbackLists[typeID].emplace_back(callback); }
		static void SubscribeToCategory(unsigned int categoryID, EventCallbackPtr callback);

		// Unsubscription functions
		static void UnsubscribeFromType(unsigned int typeID, EventCallbackPtr callback);
		static void UnsubscribeFromCategory(unsigned int categoryID, EventCallbackPtr callback);

		// Dispatch functions
		inline static void Broadcast(EventPtr e) { eventBuffer.emplace_back(e); };

		// Core Calls
		static void Init();
		static void Update();
		static void Cleanup();

	private:
		// Event buffer (all Event types, in broadcast order)
		static std::vector<EventPtr> eventBuffer;

		// Callback buffer array (Dim 1: TypeID).  Dynamically allocated.
		static std::list<EventCallbackPtr>* callbackLists;
		
		// Category map (Dim 1: CategoryID, Dim 2: TypeID).  Dynamically allocated.
		static std::vector<unsigned int>* EventCategoryMatrix;

		// Allocating both core and client resources
		static void GetClientListSizes(); // Gets configuration values used to set up Dispatcher from the client
		static void InitClientECM();
		static unsigned int m_NumOfEventTypes, m_NumOfEventCategories;
	};

	// Express create and disptach a new Event.
	#define EP_QUICKEVENT(eType, ...) Enterprise::Event::Dispatcher::Broadcast(Enterprise::Event::CreateEPEvent<eType>(__VA_ARGS__))
}
#pragma once
#include "BaseEvent.h"

/* Dispatcher.h
	The Dispatcher is Enterprise's event handling system.

	If you want to receive a broadcasted Event, you can register a callback here.  Events that are broadcasted are dispatched
	in order of priority.

	- Callbacks take the form bool OnEvent(std::shared_ptr<Event> e).
	- If a callback returns true, then the event it was handling is blocked from the rest of the callback stack.
*/

namespace Enterprise
{
	struct DispatcherInitData {
		unsigned int NumOfEventTypes, NumOfEventCategories, eventBufferSize;
	};

	class Dispatcher
	{
	public:
		// Subscription functions
		inline static void SubscribeToType(unsigned int typeID, std::function<bool(EP_EVENTPTR)> callback) {
			callbackLists[typeID].emplace_back(callback);
		}
		static void SubscribeToCategory(unsigned int categoryID, std::function<bool(EP_EVENTPTR)> callback);

		// Dispatch function
		inline static void Broadcast(EP_EVENTPTR event) { eventBuffer.emplace_back(event); }

		// Core Calls
		static void Init();
		static void Update();
	private:
		// Event buffer (all Event types, in broadcast order)
		static std::vector<EP_EVENTPTR> eventBuffer;

		// Callback buffer array (Dim 1: TypeID, Dim 2: Callbacks function pointers).  Dynamically allocated.
		static std::vector<std::function<bool(EP_EVENTPTR)>>* callbackLists;
		
		// Category map (Dim 1: CategoryID, Dim 2: TypeID).  Dynamically allocated.
		static std::vector<unsigned int>* EventCategoryMatrix;

		static void InitClientAllocation(); // Gets configuration values used to set up Dispatcher from the client
		static void InitClientECM();
		static unsigned int m_NumOfEventTypes, m_NumOfEventCategories, m_BufferSize;
	};

	// Express create and disptach a new Event.
	#define EP_QUICKEVENT(eType, ...) Enterprise::Dispatcher::Broadcast(Enterprise::Event::CreateEPEvent<eType>(__VA_ARGS__))
}
#include "EP_PCH.h"
#include "Dispatcher.h"
#include "CoreEvents.h"

#include "Enterprise/Core/Console.h"

namespace Enterprise::Event {

	// RESOURCES =========================================================================================================

	//using namespace Event;

	// Static definitions
	std::vector<EventPtr> Dispatcher::eventBuffer;
	std::list<EventCallbackPtr>* Dispatcher::callbackLists;
	//EventCallbackPtr Dispatcher::applicationCallback;
	std::vector<unsigned int>* Dispatcher::EventCategoryMatrix;
	unsigned int Dispatcher::m_NumOfEventTypes = 0, Dispatcher::m_NumOfEventCategories = 0;

	// FUNCTIONS =========================================================================================================

	// Core Calls --------------------------------------------------------------------------------------------------------
	void Dispatcher::Init()// EventCallbackPtr BaseAppCallback)
	{
		GetClientListSizes(); // Get m_NumOfEventTypes and m_NumOfEventCategories
		EventCategoryMatrix = new std::vector<unsigned int>[m_NumOfEventCategories]; //Create ECM

		// Loop through all event types and place them in the appropriate "all" category.
		for (unsigned int i = 1; i < m_NumOfEventTypes; ++i) // We start at i=1 to skip TypeIDs::_None.
		{
			switch (i)
			{
			// TypeIDs::_NumOfCoreTypes == Client's _None.  We skip it.
			case TypeIDs::_NumOfCoreTypes:
				break;
			default:
				if (i < TypeIDs::_NumOfCoreTypes)
					EventCategoryMatrix[CategoryIDs::_All].emplace_back(i);
				else
					//_NumOfCoreCategories == Client's _All.
					EventCategoryMatrix[CategoryIDs::_NumOfCoreCategories].emplace_back(i);
				break;
			}
		}
		// Populate Core Categories
		#include "Generation/StartECM.h"
		#include "CoreEvents_CategoryList.h"
		#include "Generation/Stop.h"
		// Populate Client Categories
		InitClientECM();

		// Allocate event buffer (TODO: Set size from file)
		eventBuffer.reserve(20);
		// Allocate callback lists
		callbackLists = new std::list<EventCallbackPtr>[m_NumOfEventTypes];
	}

	void Dispatcher::Update() // Every update, dispatch the Event buffer, then clear it.
	{
		// Event buffer iteration
		for (auto event_iterator = eventBuffer.begin(); event_iterator != eventBuffer.end(); ++event_iterator)
		{
			// Call all callbacks for the Event's type until it is handled
			unsigned int callbackListIndex = (*event_iterator)->GetTypeID();

			for (auto callback_iterator = callbackLists[callbackListIndex].rbegin();
				callback_iterator != callbackLists[callbackListIndex].rend();
				++callback_iterator) {

				if ((*callback_iterator)((*event_iterator)))
					break;
			}
		}
		eventBuffer.clear();
	}

	void Dispatcher::Cleanup()
	{
		delete[] callbackLists;
		delete[] EventCategoryMatrix;
	}

	// Subscription functions --------------------------------------------------------------------------------------------
	void Dispatcher::SubscribeToCategory(unsigned int categoryID, EventCallbackPtr callback) {
		for (auto eventTypeIterator = EventCategoryMatrix[categoryID].begin();
			eventTypeIterator != EventCategoryMatrix[categoryID].end();
			++eventTypeIterator)
		{
			SubscribeToType(*eventTypeIterator, callback);
			//TODO: add an error if a subscriber is already subscribed to this Event type.
		}
	}

	void Dispatcher::UnsubscribeFromType(unsigned int typeID, EventCallbackPtr callback)
	{
		for (auto callbackIterator = callbackLists[typeID].begin();
			callbackIterator != callbackLists[typeID].end();
			++callbackIterator)
		{
			// If the callback is a match, remove it.
			if ((*callbackIterator) == callback) {
				callbackLists[typeID].erase(callbackIterator);
				return;
			}
		}
		// If we haven't returned, the callback wasn't found.
		EP_WARN("Dispatcher: UnsubscribeFromType attempted on a callback which wasn't registered. TypeID: {}", typeID);
	}

	void Dispatcher::UnsubscribeFromCategory(unsigned int categoryID, EventCallbackPtr callback)
	{
		for (auto eventTypeIterator = EventCategoryMatrix[categoryID].begin();
			eventTypeIterator != EventCategoryMatrix[categoryID].end();
			++eventTypeIterator)
		{
			UnsubscribeFromType(*eventTypeIterator, callback);
		}
	}
}
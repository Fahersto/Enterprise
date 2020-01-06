#include "EP_PCH.h"
#include "Dispatcher.h"
#include "CoreEvents.h"

#include "Enterprise/Application/Console.h"

namespace Enterprise {

	// RESOURCES =========================================================================================================

	using namespace Event;

	// Static definitions
	std::vector<EP_EVENTPTR> Dispatcher::eventBuffer;
	std::list<EventCallbackPtr>* Dispatcher::callbackLists;
	std::vector<unsigned int>* Dispatcher::EventCategoryMatrix;
	unsigned int Dispatcher::m_NumOfEventTypes = 0, Dispatcher::m_NumOfEventCategories = 0;

	// FUNCTIONS =========================================================================================================

	// Core Calls --------------------------------------------------------------------------------------------------------
	void Dispatcher::Init()
	{
		// Allocate resources -------------------------------------------------------
		GetClientListSizes(); //Gets m_NumOfEventTypes and m_NumOfEventCategories
		eventBuffer.reserve(20); //Allocate Event buffer TODO: Set size from file
		callbackLists = new std::list<EventCallbackPtr>[m_NumOfEventTypes]; //Create callback lists
		EventCategoryMatrix = new std::vector<unsigned int>[m_NumOfEventCategories]; //Create ECM
		for (unsigned int i = 0; i < m_NumOfEventTypes; ++i) 
		{
			switch (i) //Populate the "All" ECM categories while we're here
			{
			case TypeIDs::_None:
				break;
			case TypeIDs::_NumOfCoreTypes:
				break;
			default:
				EventCategoryMatrix[
					i > TypeIDs::_NumOfCoreTypes ?
						CategoryIDs::_All :
						CategoryIDs::_NumOfCoreCategories].emplace_back(i); //_NumOfCoreCategories == Client's _All.
			}
		}

		// Populate Event Category Matrix -------------------------------------------
		// Core
		#include "Generation/StartECM.h"
		#include "CoreEvents_CategoryList.h"
		#include "Generation/Stop.h"
		// Client
		InitClientECM();
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
			if (*callbackIterator == callback) {
				callbackLists[typeID].erase(callbackIterator);
				return;
			}
		}
		// If we haven't returned, the callback wasn't found.
		// TODO: return stack trace?
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
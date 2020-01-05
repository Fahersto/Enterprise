#include "EP_PCH.h"
#include "Dispatcher.h"
#include "CoreEvents.h"

namespace Enterprise {

	// RESOURCES =========================================================================================================

	using namespace Event;

	// Readability helpers
	using callbackPtr = std::function<bool(EP_EVENTPTR)>;

	// Static definitions
	std::vector<EP_EVENTPTR> Dispatcher::eventBuffer;
	std::list<callbackPtr>* Dispatcher::callbackLists;
	std::vector<unsigned int>* Dispatcher::EventCategoryMatrix;
	unsigned int Dispatcher::m_NumOfEventTypes = 0, Dispatcher::m_NumOfEventCategories = 0;

	// FUNCTIONS =========================================================================================================

	// Core Calls --------------------------------------------------------------------------------------------------------
	void Dispatcher::Init()
	{
		// Allocate resources -------------------------------------------------------
		GetClientListSizes(); //Gets m_NumOfEventTypes and m_NumOfEventCategories
		eventBuffer.reserve(20); //Allocate Event buffer TODO: Set size from file
		callbackLists = new std::list<callbackPtr>[m_NumOfEventTypes]; //Create callback lists
		EventCategoryMatrix = new std::vector<unsigned int>[m_NumOfEventCategories]; //Create ECM
		for (unsigned int i = 0; i < m_NumOfEventTypes; ++i) 
		{
			// (OLD) callbackLists[i].reserve(10); //Allocate callback stack sizes TODO: Set sizes from file
			
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
		#include "Generation\StartECM.h"
		#include "CoreEvents_CategoryList.h"
		#include "Generation\StopAll.h"
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
	void Dispatcher::SubscribeToCategory(unsigned int categoryID, std::function<bool(EP_EVENTPTR)> callback) {
		for (auto eventTypeIterator = EventCategoryMatrix[categoryID].begin();
			eventTypeIterator != EventCategoryMatrix[categoryID].end();
			++eventTypeIterator)
		{
			callbackLists[size_t(*eventTypeIterator)].emplace_back(callback);
			//TODO: add a warning if a subscriber is already subscribed to this Event type.
		}
	}
	void Dispatcher::UnsubscribeFromType(unsigned int typeID, std::function<bool(EP_EVENTPTR)> callback)
	{
		for (auto callbackIterator = callbackLists[typeID].begin();
			callbackIterator != callbackLists[typeID].end();
			++callbackIterator)
		{
			// Check if the callback provided matches the iterator, and if it does, remove it.
			// You should be able to break here after removing the callback, since only one instance of a callback is 
			// allowed per Event type.
			// Be sure to add a warning if an attempt to unsubscribe is made and the callback isn't registered.
		}
	}
}
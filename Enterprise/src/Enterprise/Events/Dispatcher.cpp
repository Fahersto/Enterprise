#include "EP_PCH.h"
#include "Dispatcher.h"
#include "CoreEvents.h"

namespace Enterprise {

	using namespace Event;

	// Readability helpers
	using callbackPtr = std::function<bool(EP_EVENTPTR)>;

	// Static definitions
	std::vector<EP_EVENTPTR> Dispatcher::eventBuffer;
	std::vector<callbackPtr>* Dispatcher::callbackLists;
	std::vector<unsigned int>* Dispatcher::EventCategoryMatrix;
	unsigned int Dispatcher::m_NumOfEventTypes = 0, Dispatcher::m_NumOfEventCategories = 0, Dispatcher::m_BufferSize = 0;

	// FUNCTIONS ----------------------------------------------------------------------------------

	// Set up Dispatcher
	void Dispatcher::Init()
	{
		InitClientAllocation();

		// Reserve buffer space
		eventBuffer.reserve(m_BufferSize);
		callbackLists = new std::vector<callbackPtr>[m_NumOfEventTypes];
		EventCategoryMatrix = new std::vector<unsigned int>[m_NumOfEventCategories];
		for (unsigned int i = 0; i < m_NumOfEventTypes; ++i) { callbackLists[i].reserve(10); } //TODO: Set individual callback buffer values

		// Populate the core entries of the Event Category Matrix
		#define EVENTCATEGORY(category, ...) Dispatcher::EventCategoryMatrix[CategoryIDs::category] = std::vector<unsigned int>{__VA_ARGS__};
		#include "CoreEvents_CategoryList.h"
		#undef EVENTCATEGORY

		// Populate the client entries of the Event Category Matrix
		InitClientECM();
	}

	// Register a callback for all Event types in a category
	void Dispatcher::SubscribeToCategory(unsigned int categoryID, std::function<bool(EP_EVENTPTR)> callback)
	{
		for (auto eventTypeIterator = EventCategoryMatrix[categoryID].begin();
			eventTypeIterator != EventCategoryMatrix[categoryID].end();
			++eventTypeIterator)
		{
			callbackLists[size_t(*eventTypeIterator)].emplace_back(callback);
			//TODO: add a warning if a subscriber is already subscribed to this Event type.
		}
	}

	// Dispatch the Event buffer
	void Dispatcher::Update()
	{
		// Event buffer iteration
		for (std::vector<EP_EVENTPTR>::iterator event_iterator = eventBuffer.begin();
			event_iterator != eventBuffer.end();
			++event_iterator) {

			// Call all callbacks for the Event's type until it is handled
			unsigned int callbackListIndex = (*event_iterator)->GetTypeID();
			for (std::vector<callbackPtr>::reverse_iterator callback_iterator = callbackLists[callbackListIndex].rbegin();
				callback_iterator != callbackLists[callbackListIndex].rend();
				++callback_iterator) {

				if ((*callback_iterator)((*event_iterator)))
					break;
			}
		}
		eventBuffer.clear();
	}
}
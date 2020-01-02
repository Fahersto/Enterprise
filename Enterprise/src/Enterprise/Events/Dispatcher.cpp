#include "EP_PCH.h"
#include "Dispatcher.h"

namespace Enterprise::Event {

	// Readability helpers
	using eventPtr = std::shared_ptr<Event>;
	using callbackPtr = std::function<bool(eventPtr)>;

	// Static vector definitions
	std::vector<eventPtr> Dispatcher::eventBuffer;
	std::vector<callbackPtr> Dispatcher::callbackLists[TypeIDs::NumOfCoreTypes];
	std::array<std::vector<unsigned int>, CategoryIDs::NumOfCoreCategories> Dispatcher::EventCategoryMatrix;

	// FUNCTIONS ----------------------------------------------------------------------------------

	// Set up Dispatcher
	void Dispatcher::Init()
	{
		// Reserve buffer space
		eventBuffer.reserve(20); 
		for (int i = 0; i < TypeIDs::NumOfCoreTypes; i++) { callbackLists[i].reserve(10); }

		// Populate EventCategoryMatrix
		#define EVENTCATEGORY(category, ...) Dispatcher::EventCategoryMatrix.at(CategoryIDs::category) = std::vector<unsigned int>{__VA_ARGS__};
		#include "CoreEvents_CategoryList.h"
		#undef EVENTCATEGORY
	}

	// Register a callback for all Event types in a category
	void Dispatcher::SubscribeToCategory(unsigned int categoryID, std::function<bool(std::shared_ptr<Event>)> callback)
	{
		for (auto eventTypeIterator = EventCategoryMatrix.at(categoryID).begin();
			eventTypeIterator != EventCategoryMatrix.at(categoryID).end();
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
		for (std::vector<eventPtr>::iterator event_iterator = eventBuffer.begin();
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
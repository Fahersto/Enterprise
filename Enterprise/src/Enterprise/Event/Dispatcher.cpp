#include "EP_PCH.h"
#include "Dispatcher.h"

//extern std::vector<std::function<bool(std::shared_ptr<Enterprise::Event>)>>* getClientCallbackLists();

namespace Enterprise {

	// Readability helpers (don't use in function parameters; IntelliSense hates it)
	using eventPtr = std::shared_ptr<Event>;
	using callbackPtr = std::function<bool(eventPtr)>;

	// Static vector definitions (without these, no translation unit would have the definitions)
	std::vector<eventPtr> Dispatcher::eventBuffer;
	std::vector<callbackPtr> Dispatcher::callbackLists[(int)EventType::NumOfTypes];

	std::array<std::vector<EventType>, size_t(EventCategory::NumOfCategories)> Dispatcher::EventCategoryMatrix;

	void Dispatcher::Init()
	{
		// Reserve sizes for the event buffer
		eventBuffer.reserve(20); 
		// Reserve sizes for the callback vectors
		for (int i = 0; i < (int)EventType::NumOfTypes; i++) { callbackLists[i].reserve(10); }

		// Populate the EventCategoryMatrix
		#define EVENTCATEGORY(category, ...) Dispatcher::EventCategoryMatrix.at((size_t)EventCategory::category) = std::vector<EventType>{__VA_ARGS__};
		#include "EventCategoryList.h"
		#undef EVENTCATEGORY
	}

	void Dispatcher::Update()
	{
		// Iterate through event list
		for (std::vector<eventPtr>::iterator event_iterator = eventBuffer.begin(); event_iterator != eventBuffer.end(); ++event_iterator)
		{
			// Get the index for the given event's callback list
			size_t callbackListIndex = (size_t)(*event_iterator)->GetEventType();

			// Iterate through the correct callback list until the callback returns false. Start with the top of the stack.
			for (std::vector<callbackPtr>::reverse_iterator callback_iterator = callbackLists[callbackListIndex].rbegin();
				callback_iterator != callbackLists[callbackListIndex].rend();
				++callback_iterator)
			{
				if ((*callback_iterator)((*event_iterator))) // Call the callback
					break; // if true, event has been handled.
			}
		}

		// Clear the event buffer for the next update interval
		eventBuffer.clear();
	}
}
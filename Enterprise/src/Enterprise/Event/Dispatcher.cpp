#include "EP_PCH.h"
#include "Dispatcher.h"

extern std::vector<std::function<bool(std::shared_ptr<Enterprise::Event>)>>* getClientCallbackLists();

namespace Enterprise {

	// Don't use in Dispatcher function parameters
	using eventPtr = std::shared_ptr<Event>;
	using callbackPtr = std::function<bool(eventPtr)>;

	// Static vector definitions (without this, no translation unit would have the definitions)
	std::vector<eventPtr> Dispatcher::eventBuffer;
	std::vector<callbackPtr> Dispatcher::callbackLists[(int)EventType::NumOfTypes];

	void Dispatcher::Init()
	{
		// Reserve sizes for all vectors
		eventBuffer.reserve(20);
		
		// Iterate through the vector of callback vectors
		for (int i = 0; i < (int)EventType::NumOfTypes; i++)
		{
			callbackLists[i].reserve(10); // Reserve space for this vector of callbacks
		}
	}

	void Dispatcher::SubscribeToCategory(int category, std::function<bool(eventPtr)> callback)
	{

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
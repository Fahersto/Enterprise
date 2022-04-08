#include "Enterprise/Events.h"

using Enterprise::Events;
using Enterprise::StateManager;

std::unordered_map<HashName, std::list<std::pair<Events::EventCallbackPtr, StateManager::State*>>> Events::callbackPtrs;

Events::EventCallbackPtr Events::Subscribe(HashName type, EventCallbackPtr callback)
{
    #ifdef EP_CONFIG_DEBUG

    // For every callback already registered for this type...
    for (auto it = callbackPtrs[type].begin(); it != callbackPtrs[type].end(); ++it)
    {
        // ...check that it isn't the callback we're trying to register.
        if (it->first == callback)
        {
            EP_WARN("Events: Duplicate subscription to single event type on a callback.  "
                    "\nType: {}", HN_ToStr(type));
            EP_DEBUGBREAK();
        }
    }

    #endif
    
    callbackPtrs[type].emplace_back(std::pair(callback, StateManager::activeState));
	return callback;
}

void Events::Subscribe(std::initializer_list<HashName> types, EventCallbackPtr callback)
{
    for (const HashName& type : types)
    {
        Subscribe(type, callback);
    }
}

void Events::Unsubscribe(HashName type, EventCallbackPtr callback)
{
	for (auto it = callbackPtrs[type].begin(); it != callbackPtrs[type].end(); )
	{
		if (it->first == callback)
		{
			it = callbackPtrs[type].erase(it);
			break;
		}
		else
			it++;
	}
}

void Events::Dispatch(Event& e)
{
    // Call each registered callback until one returns true
    for (auto callbackit = callbackPtrs[e.Type()].rbegin();
         callbackit != callbackPtrs[e.Type()].rend();
         ++callbackit)
    {
		StateManager::State* prevActiveState = StateManager::activeState;
		StateManager::activeState = callbackit->second;
		bool willBreak = callbackit->first(e);
		StateManager::activeState = prevActiveState;
		if (willBreak) break;
    }
}

void Events::Dispatch(HashName type)
{
    // Generate the event
    Events::Event e = type;

    // Call each registered callback until one returns true
    for (auto callbackit = callbackPtrs[type].rbegin();
         callbackit != callbackPtrs[type].rend();
         ++callbackit)
    {
		StateManager::State* prevActiveState = StateManager::activeState;
		StateManager::activeState = callbackit->second;
		bool willBreak = callbackit->first(e);
		StateManager::activeState = prevActiveState;
		if (willBreak) break;
    }
}

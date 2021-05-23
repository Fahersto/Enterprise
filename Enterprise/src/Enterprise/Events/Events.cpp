#include "EP_PCH.h"
#include "Events.h"

namespace Enterprise
{

std::unordered_map<HashName, std::vector<Events::EventCallbackPtr>> Events::callbackPtrs;

void Events::Subscribe(HashName type, EventCallbackPtr callback)
{
    #ifndef EP_CONFIG_DIST

    // For every callback already registered for this type...
    for (auto it = callbackPtrs[type].begin(); it != callbackPtrs[type].end(); ++it)
    {
        // ...check that it isn't the callback we're trying to register.
        if ((*it) == callback)
        {
            EP_WARN("Events: Duplicate subscription to single event type on a callback.  "
                    "\nType: {}", HN_ToStr(type));
            EP_DEBUGBREAK();
        }
    }

    #endif
    
    callbackPtrs[type].emplace_back(callback);
}

void Events::Subscribe(std::initializer_list<HashName> types, EventCallbackPtr callback)
{
    for (const HashName& type : types)
    {
        Subscribe(type, callback);
    }
}

void Events::Dispatch(Event& e)
{
    // Call each registered callback until one returns true
    for (auto callbackit = callbackPtrs[e.Type()].rbegin();
         callbackit != callbackPtrs[e.Type()].rend();
         ++callbackit)
    {
        if ((*callbackit)(e))
            break;
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
        if ((*callbackit)(e))
            break;
    }
}

}

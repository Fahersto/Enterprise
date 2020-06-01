#include "EP_PCH.h"
#include "Events.h"

unsigned int _typeCount = 0;
unsigned int _categoryCount = 0;

namespace Enterprise
{

// Note: The following vectors MUST be singletons, as they are invoked during static initialization!

// Lists of callback pointers, index-aligned to event type ID.
std::vector<std::vector<Events::EventCallbackPtr>>& Events::_callbackPtrs() {
	static auto instance = new std::vector<std::vector<Events::EventCallbackPtr>>();
	return *instance;
}
// Vector associating category IDs to vectors of EventTypes.
std::vector<std::vector<Events::EventType>>& _categoryMap() {
	static auto instance = new std::vector<std::vector<Events::EventType>>();
	return *instance;
}
// Index-aligned vector associating event type IDs with their string names.
std::vector<const char*>& _typeDebugNames() {
    static auto instance = new std::vector<const char*>();
    return *instance;
}
// Index-aligned vector associating category IDs to their string names.
std::vector<const char*>& _categoryDebugNames() {
    static auto instance = new std::vector<const char*>();
    return *instance;
}


// Combine EventCategories.
Events::EventCategory Events::EventCategory::operator|(const Events::EventCategory& other) const
{
    EventCategory returnVal(*this);
    
    // For each ID in other...
    for (auto otherit = other.TypeIDs.begin(); otherit != other.TypeIDs.end(); ++otherit)
    {
        // ...check whether it exists in 'this' ...
        for (auto thisit = TypeIDs.begin(); thisit != TypeIDs.end(); ++thisit)
        {
            // ...if it does, skip it...
            if ((*otherit) == (*thisit))
                goto NextOther;
        }
        // ...if it doesn't, add it.
        returnVal.TypeIDs.emplace_back(*otherit);
        NextOther: ;
    }
    
    return returnVal;
}


// Get the stringname of an EventType.
#ifdef EP_CONFIG_DEBUG
const char* Events::GetTypeDebugName(EventType type)
{
    try { return _typeDebugNames().at(type); }
    catch (std::out_of_range)
    {
        EP_ERROR("Error: Events::GetTypeDebugName() passed unregistered event type ID.");
        return "MISSING TYPE NAME";
    }
}
#endif


// ---------------------------------------------------------------------------------------------------------------------


// Type/category registration functions

#ifdef EP_CONFIG_DEBUG

Events::EventType Events::NewType(const char* debugName)
{
    EventType returnVal = _typeCount++;         // Generate ID for this type
    _callbackPtrs().emplace_back();             // Add callback stack for type
    _typeDebugNames().emplace_back(debugName);  // Register debug name
    return returnVal;
}
Events::EventType Events::NewType(const char* debugName, EventCategory category)
{
    EventType returnVal = _typeCount++;                         // Generate ID for this type
    _callbackPtrs().emplace_back();                             // Create callback stack for type
    _typeDebugNames().emplace_back(debugName);                  // Register debug name
    _categoryMap().at(category.ID()).emplace_back(returnVal);   // Add to category map
    
    return returnVal;
}
Events::EventCategory Events::NewCategory(const char* debugName)
{
    EventCategory returnVal = _categoryCount++;     // Generate ID for this category
    _categoryDebugNames().emplace_back(debugName);  // Register debug name
    _categoryMap().emplace_back();                  // Add to category map
    return returnVal;
}

#else

Events::EventType Events::NewType()
{
    EventType returnVal = _typeCount++; // Generate ID for this type
    _callbackPtrs().emplace_back();     // Add callback stack for type
    return returnVal;
}
Events::EventType Events::NewType(EventCategory category)
{
    EventType returnVal = _typeCount++;                         // Generate ID for this type
    _callbackPtrs().emplace_back();                             // Create callback stack for type
    _categoryMap().at(category.ID()).emplace_back(returnVal);   // Add to category map
    
    return returnVal;
}
Events::EventCategory Events::NewCategory()
{
    EventCategory returnVal = _categoryCount++; // Generate ID for this category
    _categoryMap().emplace_back();              // Add to category map
    return returnVal;
}

#endif


// Subscription functions

void Events::SubscribeToType(EventType type, EventCallbackPtr callback)
{
    // For every callback already registered for this type...
    for (auto it = _callbackPtrs().at(type).begin(); it != _callbackPtrs().at(type).end(); ++it)
    {
        // ...check that it isn't the callback we're trying to register.
        if ((*it) == callback)
        {
//            EP_WARN("Events: SubscribeToType was called with a previously registered callback/type combination.  "
//                    "Type: {}", type);
            return; //TODO: Make this warning, and/or this check, optional.
        }
    }
    
    // If the callback hasn't already been registered for this type, register it.
    _callbackPtrs().at(type).emplace_back(callback);
    
}
void Events::SubscribeToCategories(EventCategory category, EventCallbackPtr callback)
{
    for ( auto it = _categoryMap().at(category.ID()).begin();
              it != _categoryMap().at(category.ID()).end();
              ++it )
        SubscribeToType(*it, callback);
}

}

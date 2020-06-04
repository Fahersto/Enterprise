#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

/* The Enterprise event system. */
class Events
{
public:

    // Identifies an event category.
    typedef unsigned int EventCategory;

    // Identifies an event type.
    typedef unsigned int EventType;

    #ifdef EP_CONFIG_DEBUG
        // Debug only: Gets the name of an EventCategory as a string.
        static const char* GetCategoryDebugName(EventCategory type);

        // Debug only: Gets the name of an EventType as a string.
        static const char* GetTypeDebugName(EventType type);
    #endif
    
    // An Enterprise event.
    class Event
    {
    public:
        Event(EventType type) : m_type(type) {}
        virtual ~Event() {}

        // Get this Event's EventType.
        inline const EventType Type() { return m_type; }

        #ifdef EP_CONFIG_DEBUG
        // Debug only: Express this Event as a string.
        virtual std::string DebugString() { return _debugName(); }
        #endif
        
    private:
        EventType m_type; // This Event's type.
    protected:
        #ifdef EP_CONFIG_DEBUG
        inline const char* _debugName() { return GetTypeDebugName(m_type); }
        #endif
    };
    
    
    // An Enterprise event with a data payload.
    template <typename T>
    class DataEvent : public Event
    {
    public:
        DataEvent(EventType type, T data) : m_data(data), Event(type) {}
        // Get this DataEvent's data payload.
        T& Data() { return m_data; }

        #ifdef EP_CONFIG_DEBUG
        // Debug only: Express this Event as a string.
        std::string DebugString()
        {
            std::stringstream ss;
            ss << _debugName() << ": " << m_data;
            return ss.str();
        }
        #endif
        
    private:
        T m_data; // This DataEvent's payload.
    };
    
    // A pointer to an event callback function.
    typedef bool(*EventCallbackPtr)(Event&);
    
    
    // ----------------------------------------------------------------------------------------
    

    // Type/category definition functions
    
    #ifdef EP_CONFIG_DEBUG

        // Register a new event category.
        static EventCategory NewCategory(const char* debugName);

        // Register a new event type.
        static EventType NewType(const char* debugName);

        // Register a new event type and put it in one or more categories.
		template<typename... Categories, typename = 
                std::enable_if_t<std::conjunction_v<std::is_same<EventCategory, Categories>...>>>
        static EventType NewType(const char* debugName, Categories ... categories)
        {
            EventType returnVal = NewType(debugName);
            ((_categoryMap().at(categories).emplace_back(returnVal)), ...);
            return returnVal;
        };

    #else

        // Register a new event category.
        static EventCategory NewCategory();

        // Register a new event type.
        static EventType NewType();

        // Register a new event type and put it in one or more categories.
        template<typename... Categories, typename =
                std::enable_if_t<std::conjunction_v<std::is_same<EventCategory, Categories>...>>>
        static EventType NewType(Categories ... categories)
        {
            EventType returnVal = NewType();
            ((_categoryMap().at(categories).emplace_back(returnVal)), ...);
            return returnVal;
        };

    #endif
    
    
    // Subscription functions
    
    // Register a callback function for all event types in a category.
    static void SubscribeToCategory(EventCategory category, EventCallbackPtr callback);

    // Register a callback function for a particular event type.
    static void SubscribeToType(EventType type, EventCallbackPtr callback);
    
    
    // Dispatch functions
    
    //Dispatch a pre-made event.
    static void Dispatch(Event& e)
    {
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(e.Type()).rbegin();
             callbackit != _callbackPtrs().at(e.Type()).rend();
                  ++callbackit)
        {
            if ((*callbackit)(e))
                break;
        }
    }
    
    // Dispatch a new event of the given type.
    static void Dispatch(EventType type)
    {
        // Generate the event
        Events::Event e = type;
        
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(type).rbegin();
                  callbackit != _callbackPtrs().at(type).rend();
                  ++callbackit)
        {
            if ((*callbackit)(e))
                break;
        }
    }


    // Dispatch a new event with a data payload.
    template <typename T>
    static void Dispatch(EventType type, T data)
    {
        // Generate the event
        Events::DataEvent<T> e = {type, data};
        
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(type).rbegin();
                  callbackit != _callbackPtrs().at(type).rend();
                  ++callbackit)
        {
            if ((*callbackit)(e))
                break;
        }
    }
    
private:
    static std::vector<std::vector<Events::EventType>>& _categoryMap();
    static std::vector<std::vector<Events::EventCallbackPtr>>& _callbackPtrs();
};

}


// Extract data from an Enterprise event.
template <typename T>
T& GetEventData(Enterprise::Events::Event& e) {
    Enterprise::Events::DataEvent<T>* converted = dynamic_cast<Enterprise::Events::DataEvent<T>*>(&e);
    EP_ASSERT(converted); //TODO: Add helpful message here.
    return converted->Data();
}


// -----------------------------------------------------------------------------------------


// Declare a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { extern const Enterprise::Events::EventCategory name; }
// Declare a new Enterprise event type.
#define EP_EVENTTYPE(name, ...) namespace EventTypes { extern const Enterprise::Events::EventType name; }


#ifdef EP_CONFIG_DEBUG

    // Define an Enterprise event category.
    #define EP_EVENTCATEGORY_DEF(name) const Enterprise::Events::EventCategory EventCategories:: name \
                                                                = Enterprise::Events::NewCategory( #name );
    // Define an Enterprise event type.
    #define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType EventTypes:: name \
                                                = Enterprise::Events::NewType( #name __VA_OPT__(,) __VA_ARGS__ );

#else

    // Define an Enterprise event category.
    #define EP_EVENTCATEGORY_DEF(name) const Enterprise::Events::EventCategory EventCategories:: name \
                                                                    = Enterprise::Events::NewCategory();
    // Define an Enterprise event type.
    #define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType EventTypes:: name \
                                                            = Enterprise::Events::NewType( __VA_ARGS__ );

#endif

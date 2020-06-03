#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

/* The Enterprise event system. */
class Events
{
public:
    
    // Identifies an event type.
    typedef unsigned int EventType;    
    #ifdef EP_CONFIG_DEBUG
    static const char* GetTypeDebugName(EventType type);
    #endif
    
    
    // Struct representing an event category.
    struct EventCategory
    {
    public:
        EventCategory(unsigned int ID) : m_ID(ID) {}
        unsigned int ID() { return m_ID; }
        
        std::vector <EventType> TypeIDs; // Vector of the types in this category.
        EventCategory operator | (const EventCategory& other) const; // Combine categories.
    private:
        unsigned int m_ID;
    };
    
    
    // An Enterprise event.
    class Event
    {
    public:
        Event(EventType type) : m_type(type) {}
        virtual ~Event() {}
        inline const EventType GetType() { return m_type; }
        
        #ifdef EP_CONFIG_DEBUG
        inline const char* DebugName() { return GetTypeDebugName(m_type); }
        virtual std::string ToString() { return DebugName(); }
        #endif
        
    private:
        EventType m_type; // This Event's type.
    };
    
    
    // An Enterprise event with a data payload.
    template <typename T>
    class DataEvent : public Event
    {
    public:
        T& Data() { return m_data; }
        
        #ifdef EP_CONFIG_DEBUG
        std::string ToString()
        {
            std::stringstream ss;
            ss << DebugName() << ": " << m_data;
            return ss.str();
        }
        #endif
        
        DataEvent(EventType type, T data) : m_data(data), Event(type) {}
    private:
        T m_data; // The payload
    };
    
    // A pointer to an event callback function.
    typedef bool(*EventCallbackPtr)(Event&);
    
    
    // ----------------------------------------------------------------------------------------
    
    // Type/category definition functions
    
    #ifdef EP_CONFIG_DEBUG
        // Establish a new event type.
        static EventType NewType(const char* debugName);
        // Establish a new event type and put it in a category.
        static EventType NewType(const char* debugName, EventCategory category);
        // Establish a new event category.
        static EventCategory NewCategory(const char* debugName);
    #else
        // Establish a new event type.
        static EventType NewType();
        // Establish a new event type and put it in a category.
        static EventType NewType(EventCategory category);
        // Establish a new event category.
        static EventCategory NewCategory();
    #endif
    
    
    // Subscription functions
    
    // Register a callback function for a particular event type.
    static void SubscribeToType(EventType type, EventCallbackPtr callback);
    // Register a callback function for all event types in a category.
    static void SubscribeToCategories(EventCategory category, EventCallbackPtr callback);
    
    
    // Dispatch functions
    
    //Dispatch a pre-made event.
    static void Dispatch(Event& e)
    {
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(e.GetType()).rbegin();
             callbackit != _callbackPtrs().at(e.GetType()).rend();
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


// Declare a new Enterprise event type.
#define EP_EVENTTYPE(name, ...) namespace EventTypes { extern const Enterprise::Events::EventType name; }
// Declare a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { extern const Enterprise::Events::EventCategory name; }


#ifdef EP_CONFIG_DEBUG
// Define an Enterprise event type.
#define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType EventTypes:: name \
                                                        = Enterprise::Events::NewType( #name , __VA_ARGS__ );
// Define an Enterprise event category.
#define EP_EVENTCATEGORY_DEF(name) const Enterprise::Events::EventCategory EventCategories:: name \
                                                            = Enterprise::Events::NewCategory( #name );

#else
// Define an Enterprise event type.
#define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType EventTypes:: name \
                                                        = Enterprise::Events::NewType( __VA_ARGS__ );
// Define an Enterprise event category.
#define EP_EVENTCATEGORY_DEF(name) const Enterprise::Events::EventCategory EventCategories:: name \
                                                                = Enterprise::Events::NewCategory();

#endif

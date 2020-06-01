#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

/* The Enterprise event system. */
class Events
{
public:
    
    // Struct representing an event type.
    struct EventType
    {
    public:
        EventType(unsigned int ID) : m_ID(ID) {}
        unsigned int ID() { return m_ID; }
    private:
        unsigned int m_ID;
        friend inline bool operator == (const EventType& left, const EventType& right) {
            return left.m_ID == right.m_ID;
        }
    };
    #ifdef EP_CONFIG_DEBUG
    static const char* GetTypeDebugName(EventType type);
    #endif
    
    
    // Struct representing an event category.
    struct EventCategory
    {
    public:
        EventCategory(unsigned int ID) : m_ID(ID) {}
        unsigned int ID() { return m_ID; }
        
        std::vector <unsigned int> TypeIDs; // List of the integer IDs of the types in this category.
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
    
//    //Dispatch a pre-made event.
//    void Dispatch(Event& e)
//    {
//        // Call each registered callback until one returns true
//        for (auto callbackit = _callbackPtrs().at(e.GetType).ID().rbegin();
//                  callbackit != _callbackPtrs().at(type.ID()).rend();
//                  ++callbackit)
//        {
//            if ((*callbackit)(e))
//                break;
//        }
//    }
    
    // Dispatch a new event of the given type.
    static void Dispatch(EventType type)
    {
        // Generate the event
        Events::Event e = type;
        
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(type.ID()).rbegin();
                  callbackit != _callbackPtrs().at(type.ID()).rend();
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
        for (auto callbackit = _callbackPtrs().at(type.ID()).rbegin();
                  callbackit != _callbackPtrs().at(type.ID()).rend();
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


// Overloading these operators allows for easy event logging.
// TODO: This is broken on macOS.
#ifdef EP_CONFIG_DEBUG
inline std::ostream& operator << (std::ostream& os, Enterprise::Events::EventType type) {
    return os << Enterprise::Events::GetTypeDebugName(type);
}
inline std::ostream& operator << (std::ostream& os, Enterprise::Events::Event e) { return os << e.ToString(); }
#endif


// Extract data from an Enterprise event.
template <typename T>
T& GetEventData(Enterprise::Events::Event& e) {
    auto converted = std::dynamic_pointer_cast<Enterprise::Events::DataEvent<T>>(e);
    EP_ASSERT(converted);
    return converted->Data();
}


// Declare a new Enterprise event type.
#define EP_EVENTTYPE(name, ...) namespace EventTypes { extern const Enterprise::Events::EventType name; }
// Declare a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { extern const Enterprise::Events::EventCategory name; }

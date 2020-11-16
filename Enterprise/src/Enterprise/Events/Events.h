#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

/// The Enterprise events system.
class Events
{
public:

    /// An Enterprise event category.
    struct EventCategory
    {
        friend class Events;
        friend bool operator == (const EventCategory& left, const EventCategory& right);

        /// Constructor.
        /// @param ID The integer ID associated with this EventCategory.
        explicit EventCategory(unsigned int ID) : m_ID(ID) {}

    private:
        unsigned int m_ID;
    };

    /// An Enterprise event type.
    struct EventType
    {
        friend class Events;
        friend bool operator == (const EventType& left, const EventType& right);

        /// Constructor.
        /// @param ID The integer ID associated with this EventType.
        explicit EventType(unsigned int ID) : m_ID(ID) {}

    private:
        unsigned int m_ID;
    };

    #ifdef EP_CONFIG_DEBUG
        /// (Debug only) Gets the name of an EventCategory as a string.
        /// @param category The EventCategory.
        /// @return The string name of the EventCategory.
        static const char* GetCategoryDebugName(EventCategory category);

        /// (Debug only) Gets the name of an EventType as a string.
        /// @param type The EventType.
        /// @return The string name of the EventType.
        static const char* GetTypeDebugName(EventType type);
    #endif
    
    /// An Enterprise event.
    class Event
    {
    public:
        /// Constructor.
        /// @param type This event's type.
        Event(EventType type) : m_type(type) {}
        virtual ~Event() {}

        /// Gets this event's EventType.
        /// @return This event's type.
        inline const EventType Type() { return m_type; }

        #ifdef EP_CONFIG_DEBUG
        /// (Debug only) Express this event as a string.
        /// @return A string representation of this event.
        virtual std::string DebugString() { return _debugName(); }
        #endif
        
    private:
        EventType m_type;

    protected:
        #ifdef EP_CONFIG_DEBUG
        /// (Debug only) Get the string name of this event's type.
        /// @return The string name of this event's type.
        inline const char* _debugName() { return GetTypeDebugName(m_type); }
        #endif
    };
    
    
    /// An Enterprise event with a data payload.
    /// @tparam T The type of the data payload.
    template <typename T>
    class DataEvent : public Event
    {
    public:
        /// Constructor.
        /// @param type This event's type.
        /// @param data The data payload.
        DataEvent(EventType type, T data) : m_data(data), Event(type) {}

        /// Gets this event's data payload.
        /// @return The data payload.
        T& Data() { return m_data; }

        #ifdef EP_CONFIG_DEBUG
        /// (Debug only) Express this event as a string.
        /// @return A string representation of this event and its payload.
        std::string DebugString()
        {
            std::stringstream ss;
            ss << _debugName() << ": " << m_data;
            return ss.str();
        }
        #endif
        
    private:
        T m_data;
    };
    
    /// A pointer to an event callback function.
    typedef bool(*EventCallbackPtr)(Event&);
    
    
    // ----------------------------------------------------------------------------------------
    

    // Type/category definition functions
    
    #ifdef EP_CONFIG_DEBUG

        /// Registers a new event category.
        /// @param debugName (Debug only) The string name of the new category.
        /// @return An EventCategory object representing the new category.
        static EventCategory NewCategory(const char* debugName);

        /// Registers a new event type.
        /// @param debugName (Debug only) The string name of the new type.
        /// @return An EventType object representing the new type.
        static EventType NewType(const char* debugName);

		/// Registers a new event type and puts it in one or more categories.
		/// @param debugName (Debug only) The string name of the new type.
		/// @param ...categories A list of EventCategorys this type belongs to.
		/// @return An EventType object representing the new type.
		template<typename... Categories, typename = 
                std::enable_if_t<std::conjunction_v<std::is_same<EventCategory, Categories>...>>>
        static EventType NewType(const char* debugName, Categories ... categories)
        {
            EventType returnVal = NewType(debugName);
            ((_categoryMap().at(static_cast<EventCategory>(categories).m_ID).emplace_back(returnVal)), ...);
            return returnVal;
        };

    #else

        /// Registers a new event category.
        /// @return An EventCategory object representing the new category.
        static EventCategory NewCategory();

        /// Registers a new event type.
        /// @return An EventType object representing the new type.
        static EventType NewType();

        /// Registers a new event type and puts it in one or more categories.
        /// @param ...categories A list of EventCategorys this type belongs to.
        /// @return An EventType object representing the new type.
        template<typename... Categories, typename =
                std::enable_if_t<std::conjunction_v<std::is_same<EventCategory, Categories>...>>>
        static EventType NewType(Categories ... categories)
        {
            EventType returnVal = NewType();
            ((_categoryMap().at(static_cast<EventCategory>(categories).m_ID).emplace_back(returnVal)), ...);
            return returnVal;
        };

    #endif
    
    
    // Subscription functions
    
    /// Registers a callback for all event types in a category.
    /// @param category The event category.
    /// @param callback A pointer to the callback function.
    static void SubscribeToCategory(EventCategory category, EventCallbackPtr callback);

    /// Registers a callback for an event type.
    /// @param type The event type.
    /// @param callback A pointer to the callback function.
    static void SubscribeToType(EventType type, EventCallbackPtr callback);
    
    
    // Dispatch functions
    
    /// Dispatches a pre-made Event.
    /// @param e A reference to the event to dispatch.
    static void Dispatch(Event& e)
    {
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(e.Type().m_ID).rbegin();
             callbackit != _callbackPtrs().at(e.Type().m_ID).rend();
                  ++callbackit)
        {
            if ((*callbackit)(e))
                break;
        }
    }
    /// Dispatches a new event of the given type.
    /// @param type The desired event type.
    static void Dispatch(EventType type)
    {
        // Generate the event
        Events::Event e = type;
        
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(type.m_ID).rbegin();
                  callbackit != _callbackPtrs().at(type.m_ID).rend();
                  ++callbackit)
        {
            if ((*callbackit)(e))
                break;
        }
    }
    /// Dispatches a new event with a data payload.
    /// @tparam T Type of the data payload.
    /// @param type The desired event type.
    /// @param data The data payload.
    template <typename T>
    static void Dispatch(EventType type, T data)
    {
        // Generate the event
        Events::DataEvent<T> e = {type, data};
        
        // Call each registered callback until one returns true
        for (auto callbackit = _callbackPtrs().at(type.m_ID).rbegin();
                  callbackit != _callbackPtrs().at(type.m_ID).rend();
                  ++callbackit)
        {
            if ((*callbackit)(e))
                break;
        }
    }


    /// Extract the data payload from a DataEvent.
    /// @tparam T The type of the data payload.
    /// @param e Reference to the Event to unpack.
    /// @return The data payload.
    /// @note C++17's structured bindings are useful for extracting data from tuples, as in:
    /// @code auto[x, y] \= Events\::Unpack&lt;std\::pair&lt;int, int&gt;&gt;(e); @endcode
    template <typename T>
    static T& Unpack(Enterprise::Events::Event& e) 
    {
        Enterprise::Events::DataEvent<T>* converted = dynamic_cast<Enterprise::Events::DataEvent<T>*>(&e);
        EP_ASSERTF(converted, "Events System: Unpack() cannot cast Event to requested DataEvent type.");
        return converted->Data();
    }

    
private:
    /// Gets the vector of EventType vectors associating each category with all its types.
    /// @return Vector of EventType vectors, index-aligned to the integer IDs of the event categories.
    /// @remarks    When a new category is registered with Events, a new vector is added to this map.
    ///             As new event types get added to each category, they get added to the category's
    ///             associated type list.  The vectors are later used by SubscribeToCategory for easy
    ///             subscriptions to all the types in a category at once.
    static std::vector<std::vector<Events::EventType>>& _categoryMap();

    /// Gets the vector of callback pointer vectors associating each type to its callbacks.
    /// @return Vector of callback pointer vectors, index-aligned to the integer IDs of each event type.
    /// @remarks    When you subscribe to an event type, the callback is appended to the type's
    ///             associated vector of callback pointers.  When an event is dispatched, it is passed
    ///             to each callback in that event type's callback vector, in reverse order, until one
    ///             of the callbacks marks it as handled.
    /// @note       When you subscribe to an event category, it appends the callback pointer to the
    ///             associated callback vector for all types in the category.
    static std::vector<std::vector<Events::EventCallbackPtr>>& _callbackPtrs();
};

}


// -----------------------------------------------------------------------------------------


#ifdef EP_CONFIG_DEBUG

    // Define an Enterprise event category.
    #define EP_EVENTCATEGORY_DEF(name) const Enterprise::Events::EventCategory name \
                                                = Enterprise::Events::NewCategory( #name )
    // Define an Enterprise event type.
    #ifdef _WIN32
        #define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType name \
                                                = Enterprise::Events::NewType( #name , __VA_ARGS__ )
    #elif defined(__APPLE__) && defined(__MACH__)
        #define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType name \
                                                = Enterprise::Events::NewType( #name __VA_OPT__(,) __VA_ARGS__ )
    #endif
    // TODO: When MSVC fixes __VA_OPT__ in C++20, use it for all platforms.

#else

    // Define an Enterprise event category.
    #define EP_EVENTCATEGORY_DEF(name) const Enterprise::Events::EventCategory name \
                                                            = Enterprise::Events::NewCategory()
    // Define an Enterprise event type.
    #define EP_EVENTTYPE_DEF(name, ...) const Enterprise::Events::EventType name \
                                                            = Enterprise::Events::NewType( __VA_ARGS__ )

#endif

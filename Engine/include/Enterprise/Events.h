#pragma once
#include <unordered_map>
#include <initializer_list>
#include <list>
#include "Enterprise/Core.h"
#include "Enterprise/StateManager.h"

namespace Enterprise
{

/// The Enterprise events system.
/// @see @ref Events
class Events
{
public:
    
    /// An Enterprise event.
    class EP_API Event
    {
    public:
        /// Constructor.
        /// @param type This event's type.
        Event(HashName type) : m_type(type) {}

        /// Get this event's type.
        /// @return This event's type.
        inline const HashName Type() { return m_type; }
        
        Event() = delete;
        virtual ~Event() {}

    private:
        HashName m_type;
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
        DataEvent(HashName type, T data) : Event(type), m_data(data) {}

        /// Get this event's data payload.
        /// @return The data payload.
        T& Data() { return m_data; }

        DataEvent() = delete;
        
    private:
        T m_data;
    };
    

    /// A pointer to an event callback function.
    typedef bool(*EventCallbackPtr)(Event&);


    /// Register a callback for an event type.
    /// @param type The HashName of the event type.
    /// @param callback A pointer to the callback function.
	/// @return @c callback.  Useful for tracking the callback address when it is a lambda expression.
    EP_API static EventCallbackPtr Subscribe(HashName type, EventCallbackPtr callback);

    /// Register a callback for multiple event types at once.
    /// @param types A list of HashNames for event types to subscribe to.
    /// @param callback A pointer to the callback function.
    EP_API static void Subscribe(std::initializer_list<HashName> types, EventCallbackPtr callback);

	/// Unregister an event callback.
	/// @param type The HashName of the event type.
	/// @param callback The pointer to the callback function.
	EP_API static void Unsubscribe(HashName type, EventCallbackPtr callback);

    
    /// Dispatch a pre-made event.
    /// @param e A reference to the event to dispatch.
    EP_API static void Dispatch(Event& e);

    /// Dispatch a new event of the given type.
    /// @param type The desired event type.
    EP_API static void Dispatch(HashName type);

    /// Dispatch a new event with a data payload.
    /// @tparam T Type of the data payload.
    /// @param type The desired event type.
    /// @param data The data payload.
    template <typename T>
    static void Dispatch(HashName type, T data)
    {
        // Generate the event
        Events::DataEvent<T> e = {type, data};
        
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


    /// Extract the data payload from a DataEvent.
    /// @tparam T The type of the data payload.
    /// @param e Reference to the event to unpack.
    /// @return The data payload.
    /// @note C++17's structured bindings are useful for extracting data from tuples, as in:
    /// @code auto[x, y] \= Events\::Unpack&lt;std\::pair&lt;int, int&gt;&gt;(e); @endcode
    template <typename T>
    static T& Unpack(Enterprise::Events::Event& e) 
    {
        Enterprise::Events::DataEvent<T>* converted = dynamic_cast<Enterprise::Events::DataEvent<T>*>(&e);
        EP_ASSERTF(converted, "Events: Unpack() cannot cast event to requested DataEvent type.");
        return converted->Data();
    }

    
private:

	// Key is the HashName of the event type.  State pointer is address of active state.
	static std::unordered_map<HashName, std::list<std::pair<Events::EventCallbackPtr, StateManager::State*>>> callbackPtrs;

};

}

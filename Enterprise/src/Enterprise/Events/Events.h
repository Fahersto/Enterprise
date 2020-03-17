#pragma once
#include "EP_PCH.h"
#include "Core.h"

// Types ==============================================================================================================
struct EventCategory //A category, or combination of categories, for Enterprise events.
{
	std::vector <unsigned int> m_IDs; //List of event category IDs represented by this EventCategory object.
	EventCategory(unsigned int ID) { m_IDs.emplace_back(ID); } //Standard constructor

	//Gets an empty EventCategory object.
	static EventCategory None() { return EventCategory(); }

	//Return an EventCategory containing all of the IDs of both EventCategory objects.
	EventCategory operator | (const EventCategory& other) const
	{
		EventCategory returnVal(*this);
		for (auto it = other.m_IDs.begin(); it != other.m_IDs.end(); ++it)
			returnVal.m_IDs.emplace_back(*it);
		return returnVal;
	}
private:
	EventCategory() {} //An empty EventCategory indicates no category assignment to a type.
};
struct EventType  //An Enterprise event type.
{
public:
	EventType(unsigned int ID) : m_ID(ID) {}
	unsigned int ID() { return m_ID; }
	friend bool operator == (const EventType& left, const EventType& right);
private:
	unsigned int m_ID;
};
bool operator == (const EventType& left, const EventType& right);

// Static Type and Category Registration Macros =======================================================================
#ifdef EP_CONFIG_DEBUG // -----------------------
// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { const EventCategory name = Enterprise::Events::RegisterCategory(#name); }
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) namespace EventTypes { const EventType name = Enterprise::Events::RegisterType(#name, categories); }
#else // ----------------------------------------
// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { const EventCategory name = Enterprise::Events::RegisterCategory(); }
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) namespace EventTypes { const EventType name = Enterprise::Events::RegisterType(categories); }
#endif // ---------------------------------------

// System classes =====================================================================================================
namespace Enterprise
{
	/* Events
		The Enterprise events system.
	*/
	class Events {
	public:
		// ----------------------------------------------------------------------------------------
		/* Event
			An Enterprise event.
		*/
		class Event : std::enable_shared_from_this<Event>
		{
		public:
			// Returns this Event's EventType.
			inline const EventType GetType() { return m_type; }

			#ifdef EP_CONFIG_DEBUG
			// Returns this Event's type as a string.
			const char* DebugName() { return GetTypeDebugName(m_type); }
			// Returns a full description of this Event as a string.
			virtual std::string ToString() { return DebugName(); }
			#endif

			Event(EventType type) : m_type(type) {}
			virtual ~Event() {}

		private:
			EventType m_type; // This Event's type.

		};

		// ----------------------------------------------------------------------------------------
		/* DataEvent
			An Enterprise event that contains data.
		*/
		template <typename T>
		class DataEvent : public Event
		{
		public:
			#ifdef EP_CONFIG_DEBUG
			std::string ToString()
			{
				std::stringstream ss;
				ss << DebugName() << ": " << m_data; //TODO: Ensure this works for containers
				return ss.str();
			}
			#endif
			T& Data() { return m_data; }

			DataEvent(EventType type, T data) : m_data(data), Event(type) {}

		private:
			T m_data; // This Event's payload.

		};

		// ----------------------------------------------------------------------------------------

		// Type registration functions

		#ifndef EP_CONFIG_DEBUG
		// Registers a new event category, and returns its associated bitfield.
		static EventCategory RegisterCategory();
		// Registers a new event type, and returns its associated ID.
		static EventType RegisterType(EventCategory categories = EventCategory::None());
		#else
		// Registers a new event category, and returns its associated bitfield.
		static EventCategory RegisterCategory(const char* debugName);
		// Registers a new event type, and returns its associated ID.
		static EventType RegisterType(const char* debugName, EventCategory categories = EventCategory::None());
		#endif

		// Debug name lookup

		#ifdef EP_CONFIG_DEBUG
		// Returns the string name of the provided EventCategory.
		static std::string GetCategoryDebugNames(EventCategory category);
		// Returns the string name of the provided EventType.
		static const char* GetTypeDebugName(EventType type);
		#endif


		// ----------------------------------------------------------------------------------------
		// Event broadcast and callback functions

		// Typedefs
		typedef std::shared_ptr<Event> EventPtr; // A smart pointer to an instance of an Enterprise event.
		typedef bool(*EventCallbackPtr)(EventPtr); // A pointer to a callback function.

		// Register a function to be called when a particular type of event occurs.
		static void SubscribeToType(EventType type, EventCallbackPtr callback);
		// Register a function to be called when an event belonging to a particular category occurs.
		static void SubscribeToCategories(EventCategory category, EventCallbackPtr callback);

		// Broadcast an event.  It will be dispatched to subscribers in order of priority.
		static void Broadcast(EventType type) 
		{
			// Generate the event
			EventPtr e = std::make_shared<Event>(type);

			// Call each registered callback until one returns true
			for (auto callbackit = _callbackPtrs().at(type.ID()).rbegin(); callbackit != _callbackPtrs().at(type.ID()).rend(); ++callbackit) {
				if ((*callbackit)(e))
					break;
			}
		}
		// Broadcast an event with a data payload.  It will be dispatched to subscribers in order of priority.
		template <typename T>
		static void Broadcast(EventType type, T data)
		{
			// Generate the event
			EventPtr e = std::make_shared<DataEvent<T>>(type, data);

			// Call each registered callback until one returns true
			for (auto callbackit = _callbackPtrs().at(type.ID()).rbegin(); callbackit != _callbackPtrs().at(type.ID()).rend(); ++callbackit) {
				if ((*callbackit)(e))
					break;
			}
		}
	private:
		// (Singleton) Vector of callback pointer lists.  Index-aligned to registered EventType underlying values.
		static std::vector<std::vector<EventCallbackPtr>>& _callbackPtrs();
	};
}

// ====================================================================================================================

// Extract data from an Enterprise EventPtr.
template <typename T>
T& GetEventData(Enterprise::Events::EventPtr e) {
	auto converted = std::dynamic_pointer_cast<Enterprise::Events::DataEvent<T>>(e);
	EP_ASSERT(converted); //Checks for failed cast.
	return converted->Data();
}

// Logging helpers
#ifdef EP_CONFIG_DEBUG
inline std::ostream& operator << (std::ostream& os, Enterprise::Events::EventPtr e) { return os << e->ToString(); }
inline std::ostream& operator << (std::ostream& os, EventCategory category) { return os << Enterprise::Events::GetCategoryDebugNames(category); }
inline std::ostream& operator << (std::ostream& os, EventType type) { return os << Enterprise::Events::GetTypeDebugName(type); }
#endif
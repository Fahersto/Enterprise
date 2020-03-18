#pragma once
#include "EP_PCH.h"
#include "Core.h"

/* Events
	Events is the Enterprise event system.  It is included as part of <Enterprise.h>.

	To access its members, invoke Enterprise::Events.  To declare new event types or event categories, use the macros
	EP_EVENTTYPE() and EP_EVENTCATEGORY().  Invoke these macros in the global namespace.
*/

// Types ==============================================================================================================

struct EventCategory //Struct representing one or more Enterprise event categories.
{
	//Gets a vector of IDs for the categories this object represents.
	std::vector<unsigned int>& IDs() { return m_IDs; };
	//Gets an empty EventCategory object.
	static EventCategory None() { return EventCategory(); }

	//Bitwise OR for combining EventCategory objects.
	EventCategory operator | (const EventCategory& other) const;

	EventCategory(unsigned int ID) { m_IDs.emplace_back(ID); }
private:
	std::vector <unsigned int> m_IDs; //List of category IDs represented by this EventCategory object.
	EventCategory() {} //An empty EventCategory indicates a lack of category assignments.
};

struct EventType  //Struct representing a type of Enterprise event.
{
	//Gets the ID for this EventType.
	unsigned int ID() { return m_ID; }
	EventType(unsigned int ID) : m_ID(ID) {}
private:
	unsigned int m_ID; //The ID for this event type.
	friend bool operator == (const EventType& left, const EventType& right);
};
bool operator == (const EventType& left, const EventType& right);


// Static Type and Category Registration Macros ---------------------------------------------------

// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { const EventCategory name = Enterprise::Events::NewCategory(#name); }
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) namespace EventTypes { const EventType name = Enterprise::Events::NewType(#name, categories); }


// ====================================================================================================================

namespace Enterprise
{	
	/* The Enterprise event system. */
	class Events {
	public:
		// ----------------------------------------------------------------------------------------
		/* An Enterprise event. */
		class Event : std::enable_shared_from_this<Event>
		{
		public:
			// Returns this Event's EventType.
			inline const EventType GetType() { return m_type; }

			// Returns this Event's type as a string.
			const char* DebugName() { return GetTypeDebugName(m_type); }
			// Returns a full description of this Event as a string.
			virtual std::string ToString() { return DebugName(); }

			Event(EventType type) : m_type(type) {}
			virtual ~Event() {}
		private:
			EventType m_type; // This Event's type.
		};

		// ----------------------------------------------------------------------------------------
		/* An Enterprise event containing a data payload. */
		template <typename T>
		class DataEvent : public Event
		{
		public:
			T& Data() { return m_data; }

			// Returns a description of this Event and its payload as a string.
			std::string ToString()
			{
				std::stringstream ss;
				ss << DebugName() << ": " << m_data;
				return ss.str();
			}

			DataEvent(EventType type, T data) : m_data(data), Event(type) {}
		private:
			T m_data; // This DataEvent's payload.
		};

		// ============================================================================================================

		// New event type and category registration

		// Registers a new event category, and returns an EventCategory struct containing the associated ID.
		static EventCategory NewCategory(const char* debugName);
		// Registers a new event type, and returns an EventType struct containing the associated ID.
		static EventType NewType(const char* debugName, EventCategory categories = EventCategory::None());

		// ----------------------------------------------------------------------------------------

		// Debug name getters

		// Gets the comma-separated string names of the categories represented by an EventCategory object.
		static std::string GetCategoryDebugNames(EventCategory category);
		// Gets the string name of the event type represented by an EventType object.
		static const char* GetTypeDebugName(EventType type);

		// ----------------------------------------------------------------------------------------

		// Event broadcast and callback functions

		// Typedefs
		typedef std::shared_ptr<Event> EventPtr; // A smart pointer to an instance of an Enterprise event.
		typedef bool(*EventCallbackPtr)(EventPtr); // A pointer to a callback function.

		// Register a callback function for a particular event type.
		static void SubscribeToType(EventType type, EventCallbackPtr callback);
		// Register a callback function for all event types in one or more categories.
		static void SubscribeToCategories(EventCategory categories, EventCallbackPtr callback);

		// Dispatch an event.  It will be dispatched to its subscribers in order of priority.
		static void Dispatch(EventType type)
		{
			// Generate the event
			EventPtr e = std::make_shared<Event>(type);

			// Call each registered callback until one returns true
			for (auto callbackit = _callbackPtrs().at(type.ID()).rbegin(); callbackit != _callbackPtrs().at(type.ID()).rend(); ++callbackit) {
				if ((*callbackit)(e))
					break;
			}
		}
		// Dispatch an event with a data payload.  It will be dispatched to its subscribers in order of priority.
		template <typename T>
		static void Dispatch(EventType type, T data)
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
		static std::vector<std::vector<EventCallbackPtr>>& _callbackPtrs(); // Singleton vector of callback pointer lists.
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
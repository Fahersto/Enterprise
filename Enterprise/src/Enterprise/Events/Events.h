#pragma once
#include "EP_PCH.h"
#include "Core.h"

// Types
typedef unsigned long long int EventCategory; // Bit field representing Enterprise event category associations TODO: move away from bitfields
typedef unsigned int EventType; // An Enterprise event type.

// Type and Category Registration Macros
#ifdef EP_CONFIG_DEBUG // -----------------------
// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { const EventCategory name = Enterprise::Events::RegisterCategory(#name); }
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) namespace EventTypes { const EventType name = Enterprise::Events::RegisterType(categories, #name); }
#else // ----------------------------------------
// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) namespace EventCategories { const EventCategory name = Enterprise::Events::RegisterCategory(); }
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) namespace EventTypes { const EventType name = Enterprise::Events::RegisterType(categories); }
#endif // ---------------------------------------

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
		// EventType and EventCategory Registration and Logging functions

		#ifdef EP_CONFIG_DEBUG
		// Registers a new event category, and returns its associated bitfield.
		static EventCategory RegisterCategory(const char* debugName);
		// Registers a new event type, and returns its associated ID.
		static EventType RegisterType(EventCategory categories, const char* debugName);

		// Returns the string name of the provided EventCategory.
		static const char* GetCategoryDebugName(EventCategory category);
		// Returns the string name of the provided EventType.
		static const char* GetTypeDebugName(EventType type);

		#else
		// Registers a new event category, and returns its associated bitfield.
		static EventCategory RegisterCategory();
		// Registers a new event type, and returns its associated ID.
		static EventType RegisterType(EventCategory categories);

		#endif

		// ----------------------------------------------------------------------------------------
		// Event callback registrastion functions


		// broadcast?
		// subscribe to type
		// subscribe to category
	};
}

// ------------------------------------------------------------------------------------------------
// Global stuff

typedef std::shared_ptr<Enterprise::Events::Event> EventPtr; // A smart pointer to an instance of an Event.

// Extract data from an Enterprise EventPtr.
template <typename T>
T& GetEventData(EventPtr e) 
{
	auto converted = std::dynamic_pointer_cast<Enterprise::Events::DataEvent<T>>(e);
	if (converted)
		return converted->Data();
}

#ifdef EP_CONFIG_DEBUG
// Directly log EventPtrs
inline std::ostream& operator << (std::ostream& os, EventPtr e) { return os << e->ToString(); }
//// Directly log EventCategorys
//inline std::ostream& operator << (std::ostream& os, EventCategory category) { return os << Enterprise::Events::GetCategoryDebugName(category); }
//// Directly log EventTypes
//inline std::ostream& operator << (std::ostream& os, EventType type) { return os << Enterprise::Events::GetTypeDebugName(type); }
#endif
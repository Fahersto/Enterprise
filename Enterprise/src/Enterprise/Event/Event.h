#pragma once
#include "EP_PCH.h"
#include "Core.h"

/* Event.h
	This header contains all of the Event objects in Enterprise.  During preprocessing, macros populate this header
	with the EventTypes and EventCategories listed in EventTypeList.h and EventCategoryList.h.
*/

namespace Enterprise {
	
	// Ennumerate the EventTypes
	enum class EventType
	{
		None = 0,
		#define EVENTTYPE(type) type, 
		#include "EventTypeList.h"
		#undef EVENTTYPE
		NumOfTypes
	};

	// Ennumerate the EventCategories
	enum class EventCategory
	{
		None = 0,
		#define EVENTCATEGORY(category, ...) category, 
		#include "EventCategoryList.h"
		#undef EVENTCATEGORY
		NumOfCategories
	};

	// Define event category bitfield constants
	#define EVENTCATEGORY(category, ...)\
		const int EventCategory_##category = BIT ((int)EventCategory::category);
	#include"EventCategoryList.h"
	#undef EVENTCATEGORY

	// Helper macros for adding boilerplate code to Event subclasses
	#ifdef EP_CONFIG_DEBUG
	#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType() { return type; }\
									virtual EventType GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override { return #type; }
	#define EVENT_CLASS_DEBUGSTRING(...)	std::string ToString() const override \
									{ \
										std::stringstream ss; \
										ss << GetName() << " " << __VA_ARGS__; \
										return ss.str(); \
									}
	#else
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return type; }\
									virtual EventType GetEventType() const override { return GetStaticType(); }
	#define EVENT_CLASS_DEBUGSTRING(...)	
	#endif
	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	// Base class for all Events.  All subclasses are procedurally generated from EventTypeList.h.
	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
		inline bool IsInCategory(int category) { return GetCategoryFlags() & category; }

		// TODO: Shared pointer getter function
		//static inline std::shared_ptr<Event> Ptr() { return std::make_shared<Event>(); }

		#ifdef EP_CONFIG_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		#endif
	};

	// Define all the EventType subclasses using macros.
	/*#define EVENTTYPE(type, ...)	template <typename... types> class Event_##type : public Event \
									{ \
									public:\
										Event_##type(__VA_ARGS__): m_Vars(__VA_ARGS__) {}\
										EVENT_CLASS_TYPE(EventType::##type)\
										EVENT_CLASS_DEBUGSTRING(m_X << ", " << m_Y)\
									private:\
										std::tuple<types...> m_Vars;
									}
	#include "EventTypeList.h"
	#undef EVENTTYPE*/

	// Output stream operator (allows direct logging of Event objects and shared_ptr's to Events)
	#ifdef EP_CONFIG_DEBUG
	inline std::ostream& operator << (std::ostream& os, const Event& e) { return os << e.ToString(); }
	inline std::ostream& operator << (std::ostream& os, std::shared_ptr<Event> e) { return os << e->ToString(); }
	#endif
}
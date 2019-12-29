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
		#define EVENTTYPE(type, ...) type, 
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
	//#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	// Base class for all Events.  All subclasses are procedurally generated from EventTypeList.h.
	class Event
	{
	public:
		
		virtual EventType GetEventType() const = 0;
		//virtual int GetCategoryFlags() const = 0;
		//inline bool IsInCategory(int category) { return GetCategoryFlags() & category; }

		// TODO: Shared pointer getter function
		//static inline std::shared_ptr<Event> Ptr() { return std::make_shared<Event>(); }

		#ifdef EP_CONFIG_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		#endif
	};

	// Output stream operator (allows direct logging of Event objects and shared_ptr's to Events)
	#ifdef EP_CONFIG_DEBUG
	inline std::ostream& operator << (std::ostream& os, const Event& e) { return os << e.ToString(); }
	inline std::ostream& operator << (std::ostream& os, std::shared_ptr<Event> e) { return os << e->ToString(); }
	#endif

	// Define all Event subclasses procedurally
	#define EVENTTYPE( ... ) VA_SELECT( EVENTTYPE, __VA_ARGS__ )
	#define EVENTTYPE_1(name) class Event_##name : public Event \
	{ \
	public: \
		EVENT_CLASS_TYPE(EventType::##name) \
	};
	#define EVENTTYPE_3(name, var1_type, var1_name) class Event_##name : public Event \
	{ \
	public: \
		Event_##name(var1_type var1_name) \
			: m_##var1_name##(var1_name) {} \
		inline var1_type Get##var1_name##() { return m_##var1_name; }\
		EVENT_CLASS_TYPE(EventType::##name) \
		EVENT_CLASS_DEBUGSTRING(#var1_name << " = " << m_##var1_name) \
	private: \
		var1_type m_##var1_name; \
	};
	#define EVENTTYPE_5(name, var1_type, var2_type, var1_name, var2_name) class Event_##name : public Event \
	{ \
	public: \
		Event_##name(var1_type var1_name, var2_type var2_name) \
			: m_##var1_name##(var1_name), m_##var2_name##(var2_name) {} \
		inline var1_type Get##var1_name##() { return m_##var1_name; }\
		inline var2_type Get##var2_name##() { return m_##var2_name; }\
		EVENT_CLASS_TYPE(EventType::##name) \
		EVENT_CLASS_DEBUGSTRING(#var1_name << " = " << m_##var1_name << ", " << #var2_name << " = " << m_##var2_name) \
	private: \
		var1_type m_##var1_name; \
		var2_type m_##var2_name; \
	};
	#define EVENTTYPE_7(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name) class Event_##name : public Event \
	{ \
	public: \
		Event_##name(var1_type var1_name, var2_type var2_name, var3_type var3_name) \
			: m_##var1_name##(var1_name), m_##var2_name##(var2_name), m_##var3_name##(var3_name) {} \
		inline var1_type Get##var1_name##() { return m_##var1_name; }\
		inline var2_type Get##var2_name##() { return m_##var2_name; }\
		inline var3_type Get##var3_name##() { return m_##var3_name; }\
		EVENT_CLASS_TYPE(EventType::##name) \
		EVENT_CLASS_DEBUGSTRING(#var1_name << " = " << m_##var1_name << ", " << #var2_name << " = " << m_##var2_name << ", " << #var3_name << " = " << m_##var3_name) \
	private: \
		var1_type m_##var1_name; \
		var2_type m_##var2_name; \
		var3_type m_##var3_name; \
	};
	#include "EventTypeList.h"
	#undef EVENTTYPE
	#undef EVENTTYPE_1
	#undef EVENTTYPE_3
	#undef EVENTTYPE_5
	#undef EVENTTYPE_7
}
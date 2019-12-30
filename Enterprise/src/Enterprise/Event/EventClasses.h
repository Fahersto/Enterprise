#pragma once
#include "Event.h"

namespace Enterprise
{
	// Macros for event logging boilerplate code
	#ifdef EP_CONFIG_DEBUG
	#define EVENTCLASS_DEBUGMEMBERS(name) \
		virtual const char* GetName() const override { return #name; }
	#define EVENTCLASS_DEBUGMEMBERS_1(name, var1_type, var1_name) \
		virtual const char* GetName() const override { return #name; } \
		std::string ToString() const override \
		{ \
			std::stringstream ss; \
			ss << GetName() << " (" << #var1_name << "=" << m_##var1_name << ")"; \
			return ss.str(); \
		}
	#define EVENTCLASS_DEBUGMEMBERS_2(name, var1_type, var2_type, var1_name, var2_name) \
		virtual const char* GetName() const override { return #name; } \
		std::string ToString() const override \
		{ \
			std::stringstream ss; \
			ss << GetName() << " (" << #var1_name << "=" << m_##var1_name << ", " << #var2_name << "=" << m_##var2_name << ")"; \
			return ss.str(); \
		}
	#define EVENTCLASS_DEBUGMEMBERS_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name) \
		virtual const char* GetName() const override { return #name; } \
		std::string ToString() const override \
		{ \
			std::stringstream ss; \
			ss << GetName() << " (" << #var1_name << "=" << m_##var1_name << ", " << \
				#var2_name << "=" << m_##var2_name << ", " << #var3_name << "=" << m_##var3_name << ")"; \
			return ss.str(); \
		}
	#else
	#define EVENTCLASS_DEBUGMEMBERS(...) 
	#define EVENTCLASS_DEBUGMEMBERS_1(...) 
	#define EVENTCLASS_DEBUGMEMBERS_2(...) 
	#define EVENTCLASS_DEBUGMEMBERS_3(...) 
	#endif

	// Event permutations
	#define EVENTTYPE(name) class Event_##name : public Event \
	{ \
	public: \
		static EventType GetStaticType() { return EventType::##name; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		EVENTCLASS_DEBUGMEMBERS(name) \
	};
	#define EVENTTYPE_1(name, var1_type, var1_name) class Event_##name : public Enterprise::Event \
	{ \
	public: \
		Event_##name(var1_type var1_name) \
			: m_##var1_name##(var1_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		static EventType GetStaticType() { return EventType::##name; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		EVENTCLASS_DEBUGMEMBERS_1(name, var1_type, var1_name) \
	private: \
		var1_type m_##var1_name; \
	};
	#define EVENTTYPE_2(name, var1_type, var2_type, var1_name, var2_name) class Event_##name : public Event \
	{ \
	public: \
		Event_##name(var1_type var1_name, var2_type var2_name) \
			: m_##var1_name##(var1_name), m_##var2_name##(var2_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		inline var2_type Get_##var2_name##() { return m_##var2_name; } \
		static EventType GetStaticType() { return EventType::##name; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		EVENTCLASS_DEBUGMEMBERS_2(name, var1_type, var2_type, var1_name, var2_name) \
	private: \
		var1_type m_##var1_name; \
		var2_type m_##var2_name; \
	};
	#define EVENTTYPE_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name) class Event_##name : public Event \
	{ \
	public: \
		Event_##name(var1_type var1_name, var2_type var2_name, var3_type var3_name) \
			: m_##var1_name##(var1_name), m_##var2_name##(var2_name), m_##var3_name##(var3_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		inline var2_type Get_##var2_name##() { return m_##var2_name; } \
		inline var3_type Get_##var3_name##() { return m_##var3_name; } \
		static EventType GetStaticType() { return EventType::##name; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		EVENTCLASS_DEBUGMEMBERS_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name) \
	private: \
		var1_type m_##var1_name; \
		var2_type m_##var2_name; \
		var3_type m_##var3_name; \
	};

	// Here's where we actually generate the classes.  The above macro definitions are applied to the contents of this header file.
	#include "EventTypeList.h"

	// Undefining all the macros used here so that they aren't visible where EventClasses.h is included.
	#undef EVENTTYPE
	#undef EVENTTYPE_1
	#undef EVENTTYPE_2
	#undef EVENTTYPE_3
	#undef EVENTCLASS_DEBUGMEMBERS
	#undef EVENTCLASS_DEBUGMEMBERS_1
	#undef EVENTCLASS_DEBUGMEMBERS_2
	#undef EVENTCLASS_DEBUGMEMBERS_3
}
#pragma once
#include "BaseEvent.h"

/* Events.h
	This class contains all of the Event subclasses.  Include this file to access events.

	These classes are generated from the list of Event types in ETL_Core.h.
*/

namespace Enterprise::Events
{
	// Macros for event logging code
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

	// Memberless classes
	#define EVENTTYPE(name) class E_##name : public Event \
	{ \
	public: \
		static EventType GetStaticType() { return EventType::##name; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		EVENTCLASS_DEBUGMEMBERS(name) \
	};
	// One-member classes
	#define EVENTTYPE_1(name, var1_type, var1_name) class E_##name : public Enterprise::Event \
	{ \
	public: \
		E_##name(var1_type var1_name) \
			: m_##var1_name##(var1_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		static EventType GetStaticType() { return EventType::##name; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		EVENTCLASS_DEBUGMEMBERS_1(name, var1_type, var1_name) \
	private: \
		var1_type m_##var1_name; \
	};
	// Two-member classes
	#define EVENTTYPE_2(name, var1_type, var2_type, var1_name, var2_name) class E_##name : public Event \
	{ \
	public: \
		E_##name(var1_type var1_name, var2_type var2_name) \
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
	// Three-member classes
	#define EVENTTYPE_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name) class E_##name : public Event \
	{ \
	public: \
		E_##name(var1_type var1_name, var2_type var2_name, var3_type var3_name) \
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

	// Here's where the classes are actually generated.  The above macro definitions are applied to the contents of this header file.
	#include "ETL_Core.h"

	// Undefining all the macros used here so that they aren't available elsewhere.
	#undef EVENTTYPE
	#undef EVENTTYPE_1
	#undef EVENTTYPE_2
	#undef EVENTTYPE_3
	#undef EVENTCLASS_DEBUGMEMBERS
	#undef EVENTCLASS_DEBUGMEMBERS_1
	#undef EVENTCLASS_DEBUGMEMBERS_2
	#undef EVENTCLASS_DEBUGMEMBERS_3
}
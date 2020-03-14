#pragma once
#include "EP_PCH.h"
#include "Core.h"
//
//typedef unsigned long long int EventCategory; // Bit field representing a combination of Enterprise event categories.
//typedef unsigned int EventType; // Type representing an Enterprise event type.
//
//namespace Enterprise
//{
//	// --------------------------------------------------------------------------------------------
//	/* Event
//		An Enterprise event.
//	*/
//	class Event
//	{
//	public:
//		// Returns this Event's EventType.
//		inline const EventType GetType() { return m_type; }
//
//		#ifdef EP_CONFIG_DEBUG
//		// Returns this Event's type as a string.
//		const char* DebugName()
//		{
//			/* TODO: Get debug name */
//		}
//
//		// Returns a full description of this Event as a string.
//		virtual std::string ToString()
//		{
//			/* TODO: Return formatted string */
//		}
//
//		#endif
//
//		Event(EventType type) : m_type(type) {}
//
//	private:
//		EventType m_type; // This Event's type.
//
//	protected:
//		virtual ~Event() {}
//	};
//
//	// --------------------------------------------------------------------------------------------
//	/* DataEvent
//		An Enterprise event that contains data.
//	*/
//	template <typename T>
//	class DataEvent : public Event
//	{
//	public:
//		#ifdef EP_CONFIG_DEBUG
//		std::string ToString()
//		{
//			/* TODO: Return formatted string */
//		}
//		#endif
//
//		DataEvent(EventType type, T data) : m_data(data), Event(type) {}
//
//	private:
//		T m_data; // This Event's payload.
//
//	};
//}
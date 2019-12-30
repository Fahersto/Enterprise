#pragma once
#include "EP_PCH.h"
#include "Core.h"

/* Event.h
	This contains the Event base class and defines used.  The base class is defined here and
	enums are generated here using macros included from EventCategoryList.h and EventTypeList.h.

	In EventTypeClasses.h, macros actually generate the subclasses from EventCategoryList.h and EventTypeList.h.
*/

namespace Enterprise {

	// Ennumerate the EventTypes
	enum class EventType
	{
		None = 0,
		#define EVENTTYPE(type, ...) type, 
		#define EVENTTYPE_1(type, ...) type, 
		#define EVENTTYPE_2(type, ...) type, 
		#define EVENTTYPE_3(type, ...) type, 
		#include "EventTypeList.h"
		#undef EVENTTYPE
		#undef EVENTTYPE_1
		#undef EVENTTYPE_2
		#undef EVENTTYPE_3
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

	// Base class for all Events.  All subclasses are procedurally generated from EventTypeList.h.
	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;

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
}
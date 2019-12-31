#pragma once
#include "EP_PCH.h"
#include "Core.h"

/* BaseEvent.h
	This header contains the Event base class, as well as the enumerations EventType and EventCategory.
	
	The EventType and EventCategory enums are generated automatically from ETL_Core.h and ECL_Core.h
	using macros.
*/

namespace Enterprise {

	enum class EventType
	{
		None = 0,
		#define EVENTTYPE(type, ...) type, 
		#define EVENTTYPE_1(type, ...) type, 
		#define EVENTTYPE_2(type, ...) type, 
		#define EVENTTYPE_3(type, ...) type, 
		#include "ETL_Core.h"
		#undef EVENTTYPE
		#undef EVENTTYPE_1
		#undef EVENTTYPE_2
		#undef EVENTTYPE_3
		NumOfTypes
	};

	enum class EventCategory
	{
		All = 0,
		#define EVENTCATEGORY(category, ...) category, 
		#include "ECL_Core.h"
		#undef EVENTCATEGORY
		NumOfCategories
	};

	// Base class for all event types.  The preprocessor generates all Event subclasses in Events.h.
	class Event : std::enable_shared_from_this<Event>
	{
	public:
		virtual EventType GetEventType() const = 0;

		// Logging functions
		#ifdef EP_CONFIG_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		#endif
	};

	// CreateEvent<>(): A helper function which instantiates a new Event, then returns an std::shared_ptr to it.
	// The typenames passed to the constructor are deductable by the compiler, so you can safely call "CreateEvent<type>(v1, v2, v3)"
	template<class eventclass, typename T1, typename T2, typename T3>
	std::shared_ptr<eventclass> CreateEvent(T1 Value1, T2 Value2, T3 Value3) { return std::make_shared<eventclass>(Value1, Value2, Value3); }
	template<class eventclass, typename T1, typename T2>
	std::shared_ptr<eventclass> CreateEvent(T1 Value1, T2 Value2 ) { return std::make_shared<eventclass>(Value1, Value2); }
	template<class eventclass, typename T1>
	std::shared_ptr<eventclass> CreateEvent(T1 Value1) { return std::make_shared<eventclass>(Value1); }
	template<class eventclass>
	std::shared_ptr<eventclass> CreateEvent() { return std::make_shared<eventclass>(); }

	// Overload the output stream operator for logging purposes
	#ifdef EP_CONFIG_DEBUG
	inline std::ostream& operator << (std::ostream& os, const Event& e) { return os << e.ToString(); }
	inline std::ostream& operator << (std::ostream& os, std::shared_ptr<Event> e) { return os << e->ToString(); }
	#endif
}
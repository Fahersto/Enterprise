#pragma once
#include "EP_PCH.h"

namespace Enterprise::Event {

	// CLASSES ------------------------------------------------------------------------------------

	// Base class for all event types.
	class Event : std::enable_shared_from_this<Event>
	{
	public:
		virtual unsigned int GetTypeID() const = 0;

		// Logging functions
		#ifdef EP_CONFIG_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		#endif
	};

	// FUNCTIONS ----------------------------------------------------------------------------------

	// Helper function which instantiates a new Event, then returns an std::shared_ptr to it.
	template<class eventclass, typename T1, typename T2, typename T3>
	std::shared_ptr<eventclass> CreateEPEvent(T1 Value1, T2 Value2, T3 Value3) { return std::make_shared<eventclass>(Value1, Value2, Value3); }
	// Helper function which instantiates a new Event, then returns an std::shared_ptr to it.
	template<class eventclass, typename T1, typename T2>
	std::shared_ptr<eventclass> CreateEPEvent(T1 Value1, T2 Value2 ) { return std::make_shared<eventclass>(Value1, Value2); }
	// Helper function which instantiates a new Event, then returns an std::shared_ptr to it.
	template<class eventclass, typename T1>
	std::shared_ptr<eventclass> CreateEPEvent(T1 Value1) { return std::make_shared<eventclass>(Value1); }
	// Helper function which instantiates a new Event, then returns an std::shared_ptr to it.
	template<class eventclass>
	std::shared_ptr<eventclass> CreateEPEvent() { return std::make_shared<eventclass>(); }

	// Overload the output stream operator for logging purposes
	#ifdef EP_CONFIG_DEBUG
	inline std::ostream& operator << (std::ostream& os, const Event& e) { return os << e.ToString(); }
	inline std::ostream& operator << (std::ostream& os, std::shared_ptr<Event> e) { return os << e->ToString(); }
	#endif
}
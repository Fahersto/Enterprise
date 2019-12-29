//#pragma once
//#include "Enterprise/Event/Event.h"
//
//namespace Sandbox {
//	// Enumerate all the client EventTypes
//	enum class EventType
//	{
//		// By starting the count here, we ensure the enumeration values are unique between core and client EventTypes
//		DONOTUSE = (int)Enterprise::EventType::NumOfTypes,
//
//		#define CLIENTEVENTTYPE(x) x, 
//		#include "SandboxETL.h"
//		#undef CLIENTEVENTTYPE
//
//		// Used by Dispatcher to allocate resources
//		NumOfTypes
//	};
//
//	// Enumerate all the client EventCategorys
//	enum class EventCategory
//	{
//		// By starting the count here, we ensure the enumeration values are unique between core and client EventCategorys
//		DONOTUSE = (int)Enterprise::EventCategory::NumOfCategories,
//
//		#define CLIENTEVENTTYPE(x) x, 
//		#include "SandboxETL.h"
//		#undef CLIENTEVENTTYPE
//
//		// Used by Dispatcher to allocate resources
//		NumOfCategories
//	};
//
//	// Define the client EventCategory bitfield constants
//	#define CLIENTEVENTCATEGORY(x) const int EventCategory_##x = BIT ((int)EventCategory::x);
//	#include"SandboxECL.h"
//	#undef CLIENTEVENTCATEGORY
//
//
//	//---------------------------------------------
//	std::vector<std::function<bool(std::shared_ptr<Enterprise::Event>)>> eventCallbackList[(int)EventType::NumOfTypes];
//
//	std::vector<std::function<bool(std::shared_ptr<Enterprise::Event>)>>* getClientCallbackLists()
//	{
//
//		return nullptr;
//	}
//}
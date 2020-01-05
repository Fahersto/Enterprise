/* StopAll.h
	Include this file after using StartEnums.h, StartClasses.h, or StartCategories.h to generate code
	relating to Event types or categories.  This file will undefine the macros used by those headers,
	allowing them to be repurposed in other areas of code.
*/

// Types
#undef EVENTTYPE
#undef EVENTTYPE_1
#undef EVENTTYPE_2
#undef EVENTTYPE_3
#undef EVENTCLASS_DEBUGMEMBERS
#undef EVENTCLASS_DEBUGMEMBERS_1
#undef EVENTCLASS_DEBUGMEMBERS_2
#undef EVENTCLASS_DEBUGMEMBERS_3

// Categories
#undef EVENTCATEGORY
/* StartECM.h
	Include this file before using a "_CategoryList.h" file to populate the Event Category Matrix.  This
	file adds defines which will convert the contents of a "_CategoryList.h" file into code which
	populates the ECM.

	WARNINGS:
	- Do not add a header guard to this file.  It is included in multiple files.
	- After you've used this file in conjunction with a "_CategoryList.h" file, you must include StopAll.h
	  to undefine these definitions.  This will allow the macros to be given different definitions elsewhere.
*/

#ifdef EP_SCOPE_CORE
#define EVENTCATEGORY(category, ...) \
	Dispatcher::EventCategoryMatrix[CategoryIDs::category] = std::vector<unsigned int>{__VA_ARGS__};
#else
#define EVENTCATEGORY(category, ...) \
	using namespace CLIENTNAMESPACE::Event; /* This is needed here so that the client category list is in scope */ \
	Dispatcher::EventCategoryMatrix[CLIENTNAMESPACE::Event::CategoryIDs::category] = std::vector<unsigned int>{__VA_ARGS__};
#endif
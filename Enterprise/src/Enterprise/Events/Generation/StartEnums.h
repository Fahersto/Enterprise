/* StartEnums.h
	Include this file before defining a TypeID or CategoryID enum for the Event system.  This file
	adds definitions which will convert the contents of a "_CategoryList.h" or a "_TypeList.h" into
	lists suitable for enumerations.

	WARNINGS:
	- Do not add a header guard to this file.  It is included in multiple files.
	- After you've used this file in conjunction with a "_CategoryList.h" or a "_TypeList.h" file,
	  you must include StopAll.h to undefine these definitions.  This will allow the macros to be
	  given different definitions elsewhere.
*/

#define EVENTTYPE(type, ...) type, 
#define EVENTTYPE_1(type, ...) type, 
#define EVENTTYPE_2(type, ...) type, 
#define EVENTTYPE_3(type, ...) type, 
#define EVENTCATEGORY(category, ...) category, 
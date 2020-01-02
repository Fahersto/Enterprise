/* EndEnums.h
	Include this file after defining a TypeID or CategoryID enum for the Event system.  This file
	undefines the definitions made in StartEnums.h, allowing "_CategoryList.h" and "_TypeList.h" files
	to be used for other purposes.

	WARNINGS:
	- Do not add a header guard to this file.  It is included in multiple files.
*/

#undef EVENTCATEGORY
#undef EVENTTYPE
#undef EVENTTYPE_1
#undef EVENTTYPE_2
#undef EVENTTYPE_3
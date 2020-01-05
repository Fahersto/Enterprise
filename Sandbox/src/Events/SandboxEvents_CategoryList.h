/* SandboxEvents_CategoryList.h
	A list of all client Event categories and the types that are in them.  Add to this list to make new
	Event category types available in the client.

	Comments directly above each macro will be visible in the enumeration's IntelliSense tooltip.  You can use
	this to write a description which will be available to anyone trying to use the category.

	WARNINGS:
	- Despite your compiler's protests, do NOT define the macros in this file.  They are defined in
	  multiple other contexts.
	- Do not add a header guard.  This file is included in multiple locations.
	- Preprocessor "\" line break rules apply.

	Macros:
	- EVENTCATEGORY(name, ...types):
	Defines a category with the given name.  Each argument after the first is the name of a TypeID in
	that category.
*/

// Turn on to disable IntelliSense errors while working in this file.
#if 0
#ifdef __INTELLISENSE__
#define EVENTCATEGORY(category, ...)
#endif
#endif

//EVENTCATEGORY(PlayerInfo, \
//	TypeIDs::PlayerPosition, \
//	TypeIDs::PlayerOOA, \
//	TypeIDs::PlayerHurt)
//EVENTCATEGORY(Health, TypeIDs::HealthAOE)
//EVENTCATEGORY(Gameplay)
//EVENTCATEGORY(Gunplay)
//EVENTCATEGORY(AI, \
//	TypeIDs::AI_Distracted, \
//	TypeIDs::AI_Curious, \
//	TypeIDs::AI_Pursuit, \
//	TypeIDs::AI_Bunkering)
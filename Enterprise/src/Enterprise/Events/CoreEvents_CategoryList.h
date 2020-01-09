/* CoreEvents_CategoryList.h
	A list of all core Event categories and the types that are in them.  Add to this list to make new
	Event category types available in the core engine.

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

// Application-related Events.
EVENTCATEGORY(Application, \
	TypeIDs::TerminateApplication)
// Window Events.
EVENTCATEGORY(Window, \
	TypeIDs::WindowClose, \
	TypeIDs::WindowMove, \
	TypeIDs::WindowResize, \
	TypeIDs::WindowFocus, \
	TypeIDs::WindowLostFocus)
// Input Events (all).
EVENTCATEGORY(Input, \
	TypeIDs::KeyDown, \
	TypeIDs::KeyUp, \
	TypeIDs::KeyChar, \
	TypeIDs::MouseButtonDown, \
	TypeIDs::MouseButtonUp, \
	TypeIDs::MouseScroll, \
	TypeIDs::MouseDelta, \
	TypeIDs::MousePosition)
// Keyboard direct input Events.
EVENTCATEGORY(Keyboard, \
	TypeIDs::KeyDown, \
	TypeIDs::KeyUp)
// Mouse Events useful when using delta movement.
EVENTCATEGORY(MouseDelta, \
	TypeIDs::MouseButtonDown, \
	TypeIDs::MouseButtonUp, \
	TypeIDs::MouseScroll, \
	TypeIDs::MouseDelta)
// Mouse Events useful when using a cursor.
EVENTCATEGORY(MouseCursor, \
	TypeIDs::MouseButtonDown, \
	TypeIDs::MouseButtonUp, \
	TypeIDs::MouseScroll, \
	TypeIDs::MousePosition)

//EVENTCATEGORY(File, \
//	TypeIDs::FileOpenRequest, \
//	TypeIDs::FileOpenComplete, \
//	TypeIDs::FileSaveRequest, \
//	TypeIDs::FileSaveComplete)
//EVENTCATEGORY(Audio, \
//	TypeIDs::PlayAudioAsset, \
//	TypeIDs::PauseAudioAsset, \
//	TypeIDs::StepAudioAsset, \
//	TypeIDs::AudioParameter, \
//	TypeIDs::StopAudioAsset)
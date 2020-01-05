/* CoreEvents_CategoryList.h
	A list of all core Event types and their member variables.  Add to this list to make new Event types
	available in the core engine.

	Comments directly above each macro will be visible in the class's IntelliSense tooltip.  You can use
	this to write a description which will be available to anyone trying to use the Event type.

	WARNINGS:
	- Despite your compiler's protests, do NOT define the macros in this file.  They are defined in
	  multiple other contexts.
	- Do not add a header guard.  This file needs to be included in multiple locations.
	- Preprocessor "\" line break rules apply.

	Macros:
	- EVENTTYPE_X(name, [opt]Member1Type, [opt]Member1Name, ... , [opt]Member3Type, [opt]Member3Name):
	Defines an Event type with the given name and members.  Every pair of arguments after the first
	argument is a type and name for a member variable.  Up to three members can be defined, but you
	must call the macro with the correct number (EVENTTYPE_1 ... EVENTTYPE_3).
*/

// Turn on to disable IntelliSense errors while working in this file.
#if 0
#ifdef __INTELLISENSE__
#define EVENTTYPE(eventtype) 
#define EVENTTYPE_1(eventtype, var1type, var1name)
#define EVENTTYPE_2(eventtype, var1type, var1name, var2type, var2name)
#define EVENTTYPE_3(eventtype, var1type, var1name, var2type, var2name, var3type, var3name)
#endif
#endif

// Application events -----------------------------------------------------------

EVENTTYPE(WindowClose)
EVENTTYPE_2(WindowMove, int, X, int, Y)
EVENTTYPE_2(WindowResize, int, X, int, Y)
EVENTTYPE(WindowFocus)
EVENTTYPE(WindowLostFocus)

// Input events -----------------------------------------------------------------
// Keyboard

EVENTTYPE_1(KeyDown, int, Code)
EVENTTYPE_1(KeyUp, int, Code)
EVENTTYPE_1(KeyChar, int, Code)

// Mouse

EVENTTYPE_1(MouseButtonDown, int, Code)
EVENTTYPE_1(MouseButtonUp, int, Code)
EVENTTYPE_2(MouseDelta, int, X, int, Y)
EVENTTYPE_2(MouseScroll, int, X, int, Y)
EVENTTYPE_2(MousePosition, int, X, int, Y)

// File events ------------------------------------------------------------------

EVENTTYPE(FileOpenRequest)
EVENTTYPE(FileOpenComplete)
EVENTTYPE(FileSaveRequest)
EVENTTYPE(FileSaveComplete)

// Audio events -----------------------------------------------------------------

EVENTTYPE(PlayAudioAsset)
EVENTTYPE(PauseAudioAsset)
EVENTTYPE(StepAudioAsset)
EVENTTYPE(AudioParameter)
EVENTTYPE(StopAudioAsset)
/* ETL_Core.h (Event Type List)
	A list of all core Event types, including names and types of their members.
	
	The preprocessor uses this list to generate the EventType enum in BaseEvent.h, and the classes themselves
	in Events.h.  To add a new Event type to the core engine, add it to this list.

	WARNINGS:
	- Despite your compiler's warnings, do NOT add a definition for the macros in this file.  This would
	  supplant the definitions in Events.h and BaseEvent.h.
	- Do NOT add "#pragma once", as this file needs to be included in multiple files.	  
	- Do not add anything to this file except for C comments and the EVENTTYPE macros.

	Macros:
	  EVENTTYPE(name): Creates an Event named "Event_[name]" with no members.
	  EVENTTYPE_1(name, VarType, VarName):  Creates an Event with one member variable.
	  EVENTTYPE_2(name, var1_type, var2_type, var1_name, var2_name): Two member variables.
	  EVENTTYPE_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name): Three members.

	Comments directly above each macro will be visible in the IntelliSense tooltip.  You can use this
	to write a description which will be available to anyone trying to use the Event type.
*/

// Application events -----------------------------------------------------------

EVENTTYPE(WindowClose)
EVENTTYPE_2(WindowMove, int, int, X, Y)
EVENTTYPE_2(WindowResize, int, int, X, Y)
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
EVENTTYPE_2(MouseDelta, int, int, X, Y)
EVENTTYPE_2(MouseScroll, int, int, Y, X)
EVENTTYPE_2(MousePosition, int, int, X, Y)

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
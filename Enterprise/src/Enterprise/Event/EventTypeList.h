/* Enterprise/Event/EventTypeList.h (Enterprise)
	This header file is a list of all the core EventTypes.  During compilation, these macros will
	be used to generate Event subclasses.  They will also be used for the EventType enumeration.

	WARNINGS:
	- Despite your compiler's warnings, do NOT add a definition for the macros in this file.  This system
	  works by defining these macros in the locations where this file is included, and any definitions here
	  will break the definitions there.
	- This file deliberately has no header guard, as it is inserted into multiple locations in Enterprise.
	  Do NOT add #pragma once.
	- Do not add anything to this file except for EVENTTYPE macros and comments.

	You can define an EventType by using the following macros:

	EVENTTYPE(name): Creates an EventType named Event_[name] with no members.
	EVENTTYPE_1(name, VarType, VarName):  Creates an EventType with one member variable and a getter.
	EVENTTYPE_2(name, var1_type, var2_type, var1_name, var2_name): Two member variables.
	EVENTTYPE_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name): Three members.
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
EVENTTYPE(Poop)
EVENTTYPE_1(BiggerPoop, int, Int)
EVENTTYPE_2(RunnyPoop, char, int, Char, Int)
EVENTTYPE_3(TacoBell, float, char, int, Float, Char, Int)
EVENTTYPE(FileSaveComplete)

// Audio events -----------------------------------------------------------------
EVENTTYPE(PlayAudioAsset)
EVENTTYPE(PauseAudioAsset)
EVENTTYPE(StepAudioAsset)
EVENTTYPE(AudioParameter)
EVENTTYPE(StopAudioAsset)
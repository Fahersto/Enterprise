/* ECL_Core.h (Event Category List)
	A list of all Event categories, and which Event types are in them.

	The preprocessor uses this list to generate the EventCategory enum in BaseEvent.h, as well as
	to generate the EventCategoryMatrix in the Dispatcher (Dispatcher.h).

	WARNINGS:
	- Despite your compiler's warnings, do NOT add a definition for the macros in this file.  This would
	  supplant the definitions in BaseEvent.h and Dispatcher.h.
	- Do NOT add "#pragma once", as this file needs to be included in multiple files.
	- Do not add anything to this file except for EVENTCATEGORY macros and comments.

	Macros:
	  EVENTCATEGORY(name, ...types): Defines a category with the given name.  Each argument after the 
	    first is added to the EventCategoryMatrix so that events are routed properly.
*/

//TODO: Remove the need for all the "EventType"s.
EVENTCATEGORY(Application, \
	EventType::WindowClose, \
	EventType::WindowMove, \
	EventType::WindowResize, \
	EventType::WindowFocus, \
	EventType::WindowLostFocus)
EVENTCATEGORY(Input, \
	EventType::KeyDown, \
	EventType::KeyUp, \
	EventType::KeyChar, \
	EventType::MouseButtonDown, \
	EventType::MouseButtonUp, \
	EventType::MouseScroll, \
	EventType::MouseDelta, \
	EventType::MousePosition)
EVENTCATEGORY(Keyboard, \
	EventType::KeyDown, \
	EventType::KeyUp)
EVENTCATEGORY(MouseDelta, \
	EventType::MouseButtonDown, \
	EventType::MouseButtonUp, \
	EventType::MouseScroll, \
	EventType::MouseDelta)
EVENTCATEGORY(MouseCursor, \
	EventType::MouseButtonDown, \
	EventType::MouseButtonUp, \
	EventType::MouseScroll, \
	EventType::MousePosition)
EVENTCATEGORY(File, \
	EventType::FileOpenRequest, \
	EventType::FileOpenComplete, \
	EventType::FileSaveRequest, \
	EventType::FileSaveComplete)
EVENTCATEGORY(Audio, \
	EventType::PlayAudioAsset, \
	EventType::PauseAudioAsset, \
	EventType::StepAudioAsset, \
	EventType::AudioParameter, \
	EventType::StopAudioAsset)
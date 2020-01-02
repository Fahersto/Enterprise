/* CoreEvents_CategoryList.h
	A list of all core Event categories and the types that are in them.  Add to this list to make new
	category types available in the core engine.

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

EVENTCATEGORY(Application, \
	TypeIDs::WindowClose, \
	TypeIDs::WindowMove, \
	TypeIDs::WindowResize, \
	TypeIDs::WindowFocus, \
	TypeIDs::WindowLostFocus)
EVENTCATEGORY(Input, \
	TypeIDs::KeyDown, \
	TypeIDs::KeyUp, \
	TypeIDs::KeyChar, \
	TypeIDs::MouseButtonDown, \
	TypeIDs::MouseButtonUp, \
	TypeIDs::MouseScroll, \
	TypeIDs::MouseDelta, \
	TypeIDs::MousePosition)
EVENTCATEGORY(Keyboard, \
	TypeIDs::KeyDown, \
	TypeIDs::KeyUp)
EVENTCATEGORY(MouseDelta, \
	TypeIDs::MouseButtonDown, \
	TypeIDs::MouseButtonUp, \
	TypeIDs::MouseScroll, \
	TypeIDs::MouseDelta)
EVENTCATEGORY(MouseCursor, \
	TypeIDs::MouseButtonDown, \
	TypeIDs::MouseButtonUp, \
	TypeIDs::MouseScroll, \
	TypeIDs::MousePosition)
EVENTCATEGORY(File, \
	TypeIDs::FileOpenRequest, \
	TypeIDs::FileOpenComplete, \
	TypeIDs::FileSaveRequest, \
	TypeIDs::FileSaveComplete)
EVENTCATEGORY(Audio, \
	TypeIDs::PlayAudioAsset, \
	TypeIDs::PauseAudioAsset, \
	TypeIDs::StepAudioAsset, \
	TypeIDs::AudioParameter, \
	TypeIDs::StopAudioAsset)
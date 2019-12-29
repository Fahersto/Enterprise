/* Enterprise/Event/EventCategoryList.h (Enterprise)
	This header file is a list of all the core EventCategorys, and what EventTypes they are associated with.
	This file deliberately has no header guard, as it is inserted into multiple locations in Enterprise.

	WARNING: despite your compiler's warnings, do not define EVENTCATEGORY in this file.  If you do, it will
	abort the definitions in the files where this header is included.
*/

//TODO: Remove the need for all the "EventType".
EVENTCATEGORY(Application, EventType::WindowClose, EventType::WindowMove, EventType::WindowResize, EventType::WindowFocus, EventType::WindowLostFocus)
EVENTCATEGORY(Input, EventType::KeyDown, EventType::KeyUp, EventType::KeyChar, EventType::MouseButtonDown, EventType::MouseButtonUp, EventType::MouseScroll, EventType::MouseDelta, EventType::MousePosition)
EVENTCATEGORY(Keyboard, EventType::KeyDown, EventType::KeyUp)
EVENTCATEGORY(MouseDelta, EventType::MouseButtonDown, EventType::MouseButtonUp, EventType::MouseScroll, EventType::MouseDelta)
EVENTCATEGORY(MouseCursor, EventType::MouseButtonDown, EventType::MouseButtonUp, EventType::MouseScroll, EventType::MousePosition)
EVENTCATEGORY(File, EventType::FileOpenRequest, EventType::FileOpenComplete, EventType::FileSaveRequest, EventType::FileSaveComplete)
EVENTCATEGORY(Audio, EventType::PlayAudioAsset, EventType::PauseAudioAsset, EventType::StepAudioAsset, EventType::AudioParameter, EventType::StopAudioAsset)
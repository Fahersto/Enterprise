/* Enterprise/Event/EventTypeList.h (Enterprise)
	This header file is a list of all the core EventTypes.  It is a list of macros which will be inserted in locations
	which will include a definition that suits that context.
	This file deliberately has no header guard, as it is inserted into multiple locations in Enterprise.

	WARNING: despite your compiler's warnings, do not define EVENTTYPE in this file.  If you do, it will
	abort the definitions in the files where this header is included.
*/

// Application events -----------------------------------------------------------
EVENTTYPE(WindowClose)
EVENTTYPE(WindowMove)
EVENTTYPE(WindowResize)
EVENTTYPE(WindowFocus)
EVENTTYPE(WindowLostFocus)

// Input events -----------------------------------------------------------------
// Keyboard
EVENTTYPE(KeyDown)
EVENTTYPE(KeyUp)
EVENTTYPE(KeyChar)
// Mouse
EVENTTYPE(MouseButtonDown)
EVENTTYPE(MouseButtonUp)
EVENTTYPE(MouseDelta)
EVENTTYPE(MouseScroll)
EVENTTYPE(MousePosition)

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
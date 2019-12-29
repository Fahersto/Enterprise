/* Enterprise/Event/EventTypeList.h (Enterprise)
	This header file is a list of all the core EventTypes.  It is a list of macros which will be inserted in locations
	which will include a definition that suits that context.
	This file deliberately has no header guard, as it is inserted into multiple locations in Enterprise.

	WARNING: despite your compiler's warnings, do not define EVENTTYPE in this file.  If you do, it will
	abort the definitions in the files where this header is included.

	EVENTTYPE(name)
	EVENTTYPE(name, var1_type, var1_name)
	EVENTTYPE(name, var1_type, var2_type, var1_name, var2_name)
	EVENTTYPE(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name)
*/

// Application events -----------------------------------------------------------
EVENTTYPE(WindowClose)
EVENTTYPE(WindowMove, int , int , X , Y)
EVENTTYPE(WindowResize , int , int , X  , Y)
EVENTTYPE(WindowFocus)
EVENTTYPE(WindowLostFocus)

// Input events -----------------------------------------------------------------
// Keyboard
EVENTTYPE(KeyDown , int , Code)
EVENTTYPE(KeyUp , int , Code)
EVENTTYPE(KeyChar , int , Code)
// Mouse
EVENTTYPE(MouseButtonDown , int , Code)
EVENTTYPE(MouseButtonUp , int , Code)
EVENTTYPE(MouseDelta , int , int , X , Y)
EVENTTYPE(MouseScroll , int , int , Y , X)
EVENTTYPE(MousePosition , int , int , X , Y)

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
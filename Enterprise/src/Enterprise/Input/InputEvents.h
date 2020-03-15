#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	EP_EVENTCATEGORY(AllInput); //All Input events.
	EP_EVENTCATEGORY(Keyboard); //Direct keyboard events (for text entry, use EventType KeyChar).
	EP_EVENTCATEGORY(MouseDelta); //Mouse input events used in pointer-less control (e.g. FPS-style).
	EP_EVENTCATEGORY(MouseCursor); //Mouse input events used in pointer-based control.

	EP_EVENTTYPE(KeyChar, EventCategories::AllInput); //A character was just entered with the keyboard.
	EP_EVENTTYPE(MousePosition, EventCategories::AllInput | EventCategories::MouseCursor); //The user has moved the cursor.
	//KeyDown
	//KeyUp
	//Mouse buttons
	//Mouse scroll wheel

	//Gamepad input
}
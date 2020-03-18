#pragma once
#include "Enterprise/Events/Events.h"

EP_EVENTCATEGORY(Keyboard); //Direct keyboard events (non-text entry).
EP_EVENTCATEGORY(MouseDelta); //Mouse input events used in pointer-less control (FPS-style).
EP_EVENTCATEGORY(MouseCursor); //Mouse input events used in pointer-based control.

EP_EVENTTYPE(KeyChar, EventCategory::None()); //A character was just entered with the keyboard.
EP_EVENTTYPE(MousePosition, EventCategories::MouseCursor); //The user has moved the cursor.

//KeyDown
//KeyUp
//Mouse buttons
//Mouse scroll wheel

//Gamepad input
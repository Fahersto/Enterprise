#pragma once
#include "Enterprise/Events/Events.h"

// Categories
EP_EVENTCATEGORY(DirectKeyboard); //Direct keyboard events (non-text entry).
EP_EVENTCATEGORY(MouseDelta); //Mouse input events used in pointer-less control (FPS-style).
EP_EVENTCATEGORY(MousePointer); //Mouse input events used in pointer-based control.

// Keyboard
EP_EVENTTYPE(KeyDown,	EventCategories::DirectKeyboard); //The user has pressed a key. | int(keycode)
EP_EVENTTYPE(KeyUp,		EventCategories::DirectKeyboard); //The user has released a key. | int(keycode)
EP_EVENTTYPE(KeyChar,	EventCategories::DirectKeyboard); //EventCategory::None()); //A character was just entered with the keyboard. | char(ASCII character)

// Mouse
EP_EVENTTYPE(MouseButtonDown,	EventCategories::MouseDelta); // | EventCategories::MousePointer); //The user has clicked a mouse button. | int(button ID)
EP_EVENTTYPE(MouseButtonUp,		EventCategories::MouseDelta); // | EventCategories::MousePointer); //The user has released a mouse button. | int(button ID)
EP_EVENTTYPE(MouseScroll,		EventCategories::MouseDelta); // | EventCategories::MousePointer); //The user has used the scroll wheel. | std::pair<int, int>(vertical delta, horizontal delta)
EP_EVENTTYPE(MouseDelta,		EventCategories::MouseDelta); //The user has moved the mouse. | std::pair<int, int>(delta x, delta y)
EP_EVENTTYPE(MousePosition,		EventCategories::MousePointer); //The user has moved the pointer. | std::pair<int, int>(position x, position y)

// Gamepad

#include "EP_PCH.h"
#include "InputEvents.h"

// Categories
EP_EVENTCATEGORY_DEF(DirectKeyboard); //Direct keyboard events (non-text entry).
EP_EVENTCATEGORY_DEF(MouseDelta); //Mouse input events used in pointer-less control (FPS-style).
EP_EVENTCATEGORY_DEF(MousePointer); //Mouse input events used in pointer-based control.

// Keyboard
EP_EVENTTYPE_DEF(KeyDown,    EventCategories::DirectKeyboard); //The user has pressed a key. | int(keycode)
EP_EVENTTYPE_DEF(KeyUp,        EventCategories::DirectKeyboard); //The user has released a key. | int(keycode)
EP_EVENTTYPE_DEF(KeyChar,    EventCategories::DirectKeyboard); //EventCategory::None()); //A character was just entered with the keyboard. | char(ASCII character)

// Mouse
EP_EVENTTYPE_DEF(MouseButtonDown,    EventCategories::MouseDelta); // | EventCategories::MousePointer); //The user has clicked a mouse button. | int(button ID)
EP_EVENTTYPE_DEF(MouseButtonUp,        EventCategories::MouseDelta); // | EventCategories::MousePointer); //The user has released a mouse button. | int(button ID)
EP_EVENTTYPE_DEF(MouseScroll,        EventCategories::MouseDelta); // | EventCategories::MousePointer); //The user has used the scroll wheel. | std::pair<int, int>(vertical delta, horizontal delta)
EP_EVENTTYPE_DEF(MouseDelta,        EventCategories::MouseDelta); //The user has moved the mouse. | std::pair<int, int>(delta x, delta y)
EP_EVENTTYPE_DEF(MousePosition,        EventCategories::MousePointer); //The user has moved the pointer. | std::pair<int, int>(position x, position y)

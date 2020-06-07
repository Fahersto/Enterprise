#pragma once
#include "Enterprise/Events/Events.h"

using Enterprise::Events;

namespace EventCategories
{

/// Direct keyboard events (non-text entry).
extern const Events::EventCategory DirectKeyboard;
/// Mouse input events used in pointer-less control (FPS-style).
extern const Events::EventCategory MouseDelta;
/// Mouse input events used in pointer-based control.
extern const Events::EventCategory MousePointer;

}

namespace EventTypes
{

// Keyboard

/// The user has pressed a key. | int keycode
extern const Events::EventType KeyDown;
/// The user has released a key. | int keycode
extern const Events::EventType KeyUp;
/// A character was just entered with the keyboard. | char asciiLetter
extern const Events::EventType KeyChar;

// Mouse

/// The user has clicked a mouse button. | int buttonID
extern const Events::EventType MouseButtonDown;
/// The user has released a mouse button. | int buttonID
extern const Events::EventType MouseButtonUp;
/// The user has used the scroll wheel. | std::pair(int vertical, int horizontal)
extern const Events::EventType MouseScroll;
/// The user has moved the mouse. | std::pair(int deltaX, int deltaY)
extern const Events::EventType MouseDelta;
/// The user has moved the pointer. | std::pair(int posX, int posY)
extern const Events::EventType MousePosition;

}


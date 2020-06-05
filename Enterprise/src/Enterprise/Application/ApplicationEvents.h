#pragma once
#include "Enterprise/Events/Events.h"

using Enterprise::Events;

namespace EventCategories
{
/// All application events.
extern const Events::EventCategory Application;
/// All window events.
extern const Events::EventCategory AllWindow;
}

namespace EventTypes
{

// Application Events

/// The user has attempted to quit the game through the OS.
extern const Events::EventType QuitRequested;

// Window Events

/// The user has clicked the close button on the game window.
extern const Events::EventType WindowClose;
/// The user has dragged the window to a new location. | std::pair<int, int>(x, y)
extern const Events::EventType WindowMove;
/// The user has resized the window. | std::pair<int, int>(width, height)
extern const Events::EventType WindowResize;
/// The game window has gained focus.
extern const Events::EventType WindowFocus;
/// The game window has lost focus.
extern const Events::EventType WindowLostFocus;

}

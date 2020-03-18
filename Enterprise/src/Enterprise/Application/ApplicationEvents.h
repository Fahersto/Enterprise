#pragma once
#include "Enterprise/Events/Events.h"

EP_EVENTCATEGORY(WindowAll); //All window events.

EP_EVENTTYPE(WindowClose, EventCategories::WindowAll); //User clicked the close button on the game window.
EP_EVENTTYPE(WindowMove, EventCategories::WindowAll); //User has dragged the window to a new location.
EP_EVENTTYPE(WindowResize, EventCategories::WindowAll); //User has resized the window.
EP_EVENTTYPE(WindowFocus, EventCategories::WindowAll); //The game window has gained focus.
EP_EVENTTYPE(WindowLostFocus, EventCategories::WindowAll); //The game window has lost focus.
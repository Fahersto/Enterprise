#pragma once
#include "Enterprise/Events/Events.h"

// Categories
EP_EVENTCATEGORY(AllWindow); //All window events.

// Window Events
EP_EVENTTYPE(WindowClose, EventCategories::AllWindow);		//The user has clicked the close button on the game window.
EP_EVENTTYPE(WindowMove, EventCategories::AllWindow);		//The user has dragged the window to a new location. | std::pair<int, int>(x, y)
EP_EVENTTYPE(WindowResize, EventCategories::AllWindow);		//The user has resized the window. | std::pair<int, int>(width, height)
EP_EVENTTYPE(WindowFocus, EventCategories::AllWindow);		//The game window has gained focus.
EP_EVENTTYPE(WindowLostFocus, EventCategories::AllWindow);	//The game window has lost focus.
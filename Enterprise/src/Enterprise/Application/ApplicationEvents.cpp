#include "EP_PCH.h"
#include "ApplicationEvents.h"

// Application Events
EP_EVENTCATEGORY_DEF(Application); //All application events.

EP_EVENTTYPE_DEF(QuitRequested, EventCategories::Application);  //The user has attempted to quit the game through the OS.

// Window Events
EP_EVENTCATEGORY_DEF(AllWindow); //All window events.
EP_EVENTTYPE_DEF(WindowClose, EventCategories::AllWindow);        //The user has clicked the close button on the game window.
EP_EVENTTYPE_DEF(WindowMove, EventCategories::AllWindow);        //The user has dragged the window to a new location. | std::pair<int, int>(x, y)
EP_EVENTTYPE_DEF(WindowResize, EventCategories::AllWindow);        //The user has resized the window. | std::pair<int, int>(width, height)
EP_EVENTTYPE_DEF(WindowFocus, EventCategories::AllWindow);        //The game window has gained focus.
EP_EVENTTYPE_DEF(WindowLostFocus, EventCategories::AllWindow);    //The game window has lost focus.

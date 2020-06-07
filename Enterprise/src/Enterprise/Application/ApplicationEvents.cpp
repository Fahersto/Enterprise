#include "EP_PCH.h"
#include "ApplicationEvents.h"

namespace EventCategories
{
EP_EVENTCATEGORY_DEF(Window);
}

namespace EventTypes
{

// Application Events
EP_EVENTTYPE_DEF(QuitRequested);

// Window Events
EP_EVENTTYPE_DEF(WindowClose,       EventCategories::Window);
EP_EVENTTYPE_DEF(WindowMove,        EventCategories::Window);
EP_EVENTTYPE_DEF(WindowResize,      EventCategories::Window);
EP_EVENTTYPE_DEF(WindowFocus,       EventCategories::Window);
EP_EVENTTYPE_DEF(WindowLostFocus,   EventCategories::Window);

}

#include "EP_PCH.h"
#include "WindowEvents.h"

namespace EventCategories
{

EP_EVENTCATEGORY_DEF(Window);

}

namespace EventTypes
{

EP_EVENTTYPE_DEF(WindowClose,       EventCategories::Window);
EP_EVENTTYPE_DEF(WindowMove,        EventCategories::Window);
EP_EVENTTYPE_DEF(WindowResize,      EventCategories::Window);
EP_EVENTTYPE_DEF(WindowFocus,       EventCategories::Window);
EP_EVENTTYPE_DEF(WindowLostFocus,   EventCategories::Window);

}

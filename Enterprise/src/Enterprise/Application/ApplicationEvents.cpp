#include "EP_PCH.h"
#include "ApplicationEvents.h"

namespace EventCategories
{
EP_EVENTCATEGORY_DEF(Application);
EP_EVENTCATEGORY_DEF(AllWindow);
}

namespace EventTypes
{

// Application Events
EP_EVENTTYPE_DEF(QuitRequested,     EventCategories::Application);

// Window Events
EP_EVENTTYPE_DEF(WindowClose,       EventCategories::AllWindow);
EP_EVENTTYPE_DEF(WindowMove,        EventCategories::AllWindow);
EP_EVENTTYPE_DEF(WindowResize,      EventCategories::AllWindow);
EP_EVENTTYPE_DEF(WindowFocus,       EventCategories::AllWindow);
EP_EVENTTYPE_DEF(WindowLostFocus,   EventCategories::AllWindow);

}

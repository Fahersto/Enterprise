#include "EP_PCH.h"
#include "InputEvents.h"

namespace EventCategories
{
EP_EVENTCATEGORY_DEF(DirectKeyboard);
EP_EVENTCATEGORY_DEF(MouseDelta);
EP_EVENTCATEGORY_DEF(MousePointer);
EP_EVENTCATEGORY_DEF(Connectivity);
}

namespace EventTypes
{

// Keyboard
EP_EVENTTYPE_DEF(KeyDown,   EventCategories::DirectKeyboard);
EP_EVENTTYPE_DEF(KeyUp,     EventCategories::DirectKeyboard);
EP_EVENTTYPE_DEF(KeyChar);

// Mouse
EP_EVENTTYPE_DEF(MouseButtonDown,   EventCategories::MouseDelta,    EventCategories::MousePointer);
EP_EVENTTYPE_DEF(MouseButtonUp,     EventCategories::MouseDelta,    EventCategories::MousePointer);
EP_EVENTTYPE_DEF(MouseScroll,       EventCategories::MouseDelta,    EventCategories::MousePointer);
EP_EVENTTYPE_DEF(MouseDelta,        EventCategories::MouseDelta);
EP_EVENTTYPE_DEF(MousePosition,                                     EventCategories::MousePointer);

// Connectivity
EP_EVENTTYPE_DEF(ControllerWake,		EventCategories::Connectivity);
EP_EVENTTYPE_DEF(ControllerDisconnect,	EventCategories::Connectivity);

}

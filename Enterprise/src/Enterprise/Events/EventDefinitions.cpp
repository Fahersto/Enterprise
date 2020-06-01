#include "EP_PCH.h"
#include "Enterprise/Application/ApplicationEvents.h"
#include "Enterprise/Input/InputEvents.h"

// Eventually, I'll need to automatically generate this file.  For now, I will do it manually.

#ifdef EP_CONFIG_DEBUG

// Application Events
const Enterprise::Events::EventCategory EventCategories::Application = Enterprise::Events::NewCategory("Application");
const Enterprise::Events::EventType EventTypes::QuitRequested = Enterprise::Events::NewType("QuitRequested", EventCategories::Application);

// Window Events
const Enterprise::Events::EventCategory EventCategories::AllWindow = Enterprise::Events::NewCategory("AllWindow");
const Enterprise::Events::EventType EventTypes::WindowClose = Enterprise::Events::NewType("WindowClose", EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowMove = Enterprise::Events::NewType("WindowMove", EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowResize = Enterprise::Events::NewType("WindowResize", EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowFocus = Enterprise::Events::NewType("WindowFocus", EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowLostFocus = Enterprise::Events::NewType("WindowClose", EventCategories::AllWindow);


// Categories
const Enterprise::Events::EventCategory EventCategories::DirectKeyboard = Enterprise::Events::NewCategory("DirectKeyboard");
const Enterprise::Events::EventCategory EventCategories::MouseDelta = Enterprise::Events::NewCategory("MouseDelta");
const Enterprise::Events::EventCategory EventCategories::MousePointer = Enterprise::Events::NewCategory("MousePointer");

// Keyboard
const Enterprise::Events::EventType EventTypes::KeyDown = Enterprise::Events::NewType("KeyDown", EventCategories::DirectKeyboard);
const Enterprise::Events::EventType EventTypes::KeyUp = Enterprise::Events::NewType("KeyUp", EventCategories::DirectKeyboard);
const Enterprise::Events::EventType EventTypes::KeyChar = Enterprise::Events::NewType("KeyChar", EventCategories::DirectKeyboard);

// Mouse
const Enterprise::Events::EventType EventTypes::MouseButtonDown = Enterprise::Events::NewType("MouseButtonDown", EventCategories::MousePointer);
const Enterprise::Events::EventType EventTypes::MouseButtonUp = Enterprise::Events::NewType("MouseButtonUp", EventCategories::MousePointer);
const Enterprise::Events::EventType EventTypes::MouseScroll = Enterprise::Events::NewType("MouseScroll", EventCategories::MousePointer);
const Enterprise::Events::EventType EventTypes::MouseDelta = Enterprise::Events::NewType("MouseDelta", EventCategories::MouseDelta);
const Enterprise::Events::EventType EventTypes::MousePosition = Enterprise::Events::NewType("MousePosition", EventCategories::MousePointer);

#else

// Application Events
const Enterprise::Events::EventCategory EventCategories::Application = Enterprise::Events::NewCategory();
const Enterprise::Events::EventType EventTypes::QuitRequested = Enterprise::Events::NewType(EventCategories::Application);

// Window Events
const Enterprise::Events::EventCategory EventCategories::AllWindow = Enterprise::Events::NewCategory();
const Enterprise::Events::EventType EventTypes::WindowClose = Enterprise::Events::NewType(EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowMove = Enterprise::Events::NewType(EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowResize = Enterprise::Events::NewType(EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowFocus = Enterprise::Events::NewType(EventCategories::AllWindow);
const Enterprise::Events::EventType EventTypes::WindowLostFocus = Enterprise::Events::NewType(EventCategories::AllWindow);


// Categories
const Enterprise::Events::EventCategory EventCategories::DirectKeyboard = Enterprise::Events::NewCategory();
const Enterprise::Events::EventCategory EventCategories::MouseDelta = Enterprise::Events::NewCategory();
const Enterprise::Events::EventCategory EventCategories::MousePointer = Enterprise::Events::NewCategory();

// Keyboard
const Enterprise::Events::EventType EventTypes::KeyDown = Enterprise::Events::NewType(EventCategories::DirectKeyboard);
const Enterprise::Events::EventType EventTypes::KeyUp = Enterprise::Events::NewType(EventCategories::DirectKeyboard);
const Enterprise::Events::EventType EventTypes::KeyChar = Enterprise::Events::NewType(EventCategories::DirectKeyboard);

// Mouse
const Enterprise::Events::EventType EventTypes::MouseButtonDown = Enterprise::Events::NewType(EventCategories::MousePointer);
const Enterprise::Events::EventType EventTypes::MouseButtonUp = Enterprise::Events::NewType(EventCategories::MousePointer);
const Enterprise::Events::EventType EventTypes::MouseScroll = Enterprise::Events::NewType(EventCategories::MousePointer);
const Enterprise::Events::EventType EventTypes::MouseDelta = Enterprise::Events::NewType(EventCategories::MouseDelta);
const Enterprise::Events::EventType EventTypes::MousePosition = Enterprise::Events::NewType(EventCategories::MousePointer);

#endif

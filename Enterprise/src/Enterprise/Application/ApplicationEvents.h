#pragma once
#include "Enterprise/Events/Events.h"

using Enterprise::Events;

namespace EventTypes
{
// Application Events

/// The user has attempted to quit the game from the OS.
extern const Events::EventType QuitRequested;

}

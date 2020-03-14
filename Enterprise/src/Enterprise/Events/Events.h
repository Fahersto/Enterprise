#pragma once
#include "EP_PCH.h"
#include "Core.h"
#include "EventClasses.h"

// TODO: Should these be put in namespaces?
#ifdef EP_CONFIG_DEBUG
// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) const EventCategory name = Enterprise::Events::RegisterCategory(#name)
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) const EventType name = Enterprise::Events::RegisterType(categories, #name)

#else
// Define a new Enterprise event category.
#define EP_EVENTCATEGORY(name) const EventCategory name = Enterprise::Events::RegisterCategory()
// Define a new Enterprise event type.
#define EP_EVENTTYPE(name, categories) const EventType name = Enterprise::Events::RegisterType(categories)

#endif

namespace Enterprise::Events
{
	#ifdef EP_CONFIG_DEBUG
	// Registers a new event category, and returns its associated bitfield.
	EventCategory RegisterCategory(const char* debugName);
	// Registers a new event type, and returns its associated ID.
	EventType RegisterType(EventCategory categories, const char* debugName);

	#else
	// Registers a new event category, and returns its associated bitfield.
	EventCategory RegisterCategory();
	// Registers a new event type, and returns its associated ID.
	EventType RegisterType(EventCategory categories);

	#endif

	// broadcast?
	// subscribe to type
	// subscribe to category
}
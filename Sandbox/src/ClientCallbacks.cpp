#include"EP_PCH.h"
#include <Enterprise.h>

/* ClientCallbacks.cpp
	This file contains external "getter" and "setter" functions required by the Enterprise core library.

	This is sort of an experiment.  At the time of this writing, the only purpose of this file is to
	provide a place for Event::Dispatcher to run Init() using values set by the client.  I may remove
	or refactor this later.

*/

#include "Events\SandboxEvents.h"

void Enterprise::Dispatcher::InitClientAllocation()  // TODO: Set from config file?
{
	// Used to allocate correct number of callback buffers
	m_NumOfEventTypes = Sandbox::Event::TypeIDs::NumOfTypes;
	m_NumOfEventCategories = Sandbox::Event::CategoryIDs::NumOfCategories;

	// Used to specify initial allocation of buffers.
	m_BufferSize = 20;
}

void Enterprise::Dispatcher::InitClientECM()
{
	// Populate the client entries of the Event Category Matrix
	using namespace Sandbox::Event;
	#define EVENTCATEGORY(category, ...) Dispatcher::EventCategoryMatrix[Sandbox::Event::CategoryIDs::category] = \
											std::vector<unsigned int>{__VA_ARGS__};
	#include "Events\SandboxEvents_CategoryList.h"
	#undef EVENTCATEGORY
}
#pragma once

/* Enterprise.h
	This file includes all of the core engine library headers needed by the client.  It shouldn't be
	necessary for a client code file to include other headers from the Enterprise engine library.
	
	This file should not be included anywhere in the engine library.
*/

//Core defines and macros
#include "Core.h"

//Application
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/Window.h"

//Event System
#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Events/CoreEvents.h"

//Time System
#include "Enterprise/Time/Time.h"
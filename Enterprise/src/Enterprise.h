#pragma once

/* Enterprise.h
	This header includes all headers needed by the client.  All a client should need to include will be included when
	including this file.
	
	This file should not be included anywhere in Enterprise.  This should only be included externally.
*/

//Core defines and macros
#include "Core.h"

//Application
#include "Enterprise/Application/Console.h"
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/Window.h"

//Event System
#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Events/CoreEvents.h"
#pragma once
/* Core.h
	This file contains engine-wide defines, classes, and macros.  This is designed to be accessible 
	from all locations in the core engine library.
*/

// Error handling
#include "Enterprise/Core/Assertions.h"
#include "Enterprise/Core/Exceptions.h"

// Logging / Reporting
#include "Enterprise/Core/Console.h"
#include "Enterprise/Core/ErrorMessageBox.h"

// Quickly assemble bit fields
#define BIT(x) (1 << x)
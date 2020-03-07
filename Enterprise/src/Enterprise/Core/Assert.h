#pragma once
#include "EP_PCH.h"
#include "Enterprise/Core/ErrorMessageBox.h"

/* Assert.h
	This header contains the Enterprise assert macros (EP_ASSERT).  It is included everywhere via Core.h inclusion.
	The intended use of EP_ASSERT is to catch situations which should NEVER occur in distribution builds.
	- In Debug, a breakpoint is triggered in the debugger, the failure is logged, and the application quits.
	- In Release, an error message dialog appears with the source code location of the failed assertion.
	- In Distribution, the assertion is stripped out.  The code wrapped in EP_ASSERT is stil called.
*/

#ifdef EP_CONFIG_DEBUG
// Break on line, log to console, then terminate.
#define EP_ASSERT(condition) do { \
	if(!(condition)) { \
		EP_FATAL("Assertion failed: {}\n{} on line {}", #condition, __FILE__, __LINE__);\
		__debugbreak(); /*TODO: Replace with portable break function*/\
		throw Enterprise::Exceptions::AssertFailed();\
	}\
} while(0)

#elif EP_CONFIG_RELEASE
// Generate error pop-up (halting thread), then terminate.
#ifdef EP_SCOPE_CORE
#define EP_ASSERT(condition) do { \
	if(!(condition)) { \
		std::wstringstream messagestream; \
		messagestream << "Assertion failed in Core: " << #condition << "\n" << __FILE__ << " on line " << __LINE__ << "\n\n\
The Enterprise engine library has encountered a condition that should never occur.  It has terminated the application.";\
		Enterprise::Platform::DisplayErrorDialog(messagestream.str());\
		throw Enterprise::Exceptions::AssertFailed();\
	} \
} while(0)
#else
#define EP_ASSERT(condition) do { \
	if(!(condition)) { \
		std::wstringstream messagestream; \
		messagestream << "Assertion failed in Client: " << #condition << "\n" << __FILE__ << " on line " << __LINE__ << "\n\n\
This Enterprise application has encountered a condition that should never occur.  It has terminated itself.";\
		Enterprise::Platform::DisplayErrorDialog(messagestream.str());\
		throw Enterprise::Exceptions::AssertFailed();\
	} \
} while(0)
#endif

#elif EP_CONFIG_DIST
// Strip the assertion from distribution builds, but still evaluate the expression.
#define EP_ASSERT(condition) condition
#endif
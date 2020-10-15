#pragma once
#include "EP_PCH.h"
#include "ErrorMessageBox.h"

/* Assert.h
	This header contains the Enterprise assert macros (EP_ASSERT).  It is included everywhere via Core.h inclusion.
	The intended use of EP_ASSERT is to catch situations which should NEVER occur in distribution builds.
	- In Debug, a breakpoint is triggered in the debugger, the failure is logged, and the application quits.
	- In Release, an error message dialog appears with the source code location of the failed assertion.
	- In Distribution, the assertion is stripped out.  The code wrapped in EP_ASSERT is still called.
*/


#ifdef _WIN32
/// Triggers a breakpoint in the debugger.
#define EP_DEBUGBREAK() __debugbreak()
#elif defined(__APPLE__) && defined(__MACH__)
/// Triggers a breakpoint in the debugger.
#define EP_DEBUGBREAK() raise(SIGTRAP)
#endif


#ifdef EP_CONFIG_DEBUG // Log to the console, break on the assertion line, then terminate.


/// Assert that the given condition is true.
#define EP_ASSERT(condition) \
	do { \
		if(!(condition)) { \
			EP_FATAL("Assertion failed!\nExpression: {}\nFile: {}\nLine: {}", #condition, __FILE__, __LINE__); \
			EP_DEBUGBREAK(); \
			throw Enterprise::Exceptions::AssertFailed(); \
		}\
	} while(0)



#elif EP_CONFIG_DEV // Generate modal error pop-up then terminate.


#ifdef EP_SCOPE_CORE
/// Assert that the given condition is true.
#define EP_ASSERT(condition) \
	do { \
		if(!(condition)) { \
			std::wstringstream messagestream; \
			messagestream << "Core assertion failed!\nExpression: " << #condition << "\nFile: " << __FILE__ << \
				"\nLine: " << __LINE__ << "\n\nThe Enterprise engine library has encountered a condition that " \
				"should never occur.  It has terminated the application."; \
			Enterprise::Platform::DisplayErrorDialog(messagestream.str().c_str()); \
			throw Enterprise::Exceptions::AssertFailed(); \
		} \
	} while(0)

#elif EP_SCOPE_CLIENT
/// Assert that the given condition is true.
#define EP_ASSERT(condition) \
	do { \
		if(!(condition)) { \
			std::wstringstream messagestream; \
			messagestream << "Client assertion failed!\nExpression: " << #condition << "\nFile: " << __FILE__ << \
				"\nLine: " << __LINE__ << "\n\nThis Enterprise application has encountered a condition that " \
				"should never occur.  It has terminated itself."; \
			Enterprise::Platform::DisplayErrorDialog(messagestream.str().c_str()); \
			throw Enterprise::Exceptions::AssertFailed(); \
		} \
	} while(0)

#endif


#elif EP_CONFIG_DIST // Strip the assertion from distribution builds, but still evaluate the expression.


/// Assert that the given condition is true.
#define EP_ASSERT(condition) condition


#endif

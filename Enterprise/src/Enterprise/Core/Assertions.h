#pragma once
#include "EP_PCH.h"
#include "ErrorMessageBox.h"

/* Assertions.h
	Enterprise's assertion macros.  This file is included in Core.h.

	There are two main kinds of assertions: EP_ASSERT and EP_VERIFY.  EP_ASSERTs
	are stripped out of Dist builds completely, while EP_VERIFYs evaluate to just
	the test expression in Dist builds.

	Versions of the macros appended with "F" allow you to pass a message as a
	second parameter.

	Versions of the macros appended with "_SLOW" are stripped from both Dev and
	Dist builds.

	When an assertion fails, an error message is logged and, if a debugger is 
	attached, a breakboint is triggered.  In Dev builds, a modal pop-up is
	generated as well.
*/


#ifdef _WIN32
/// Portably triggers a breakpoint in the debugger.
#define EP_DEBUGBREAK() (__nop(), __debugbreak())
#elif defined(__APPLE__) && defined(__MACH__)
/// Portably triggers a breakpoint in the debugger.
#define EP_DEBUGBREAK() __asm__("int $3")
#endif


#ifdef EP_CONFIG_DEBUG
	#define EP_ASSERTIONS_ENABLED
	#define EP_SLOWASSERTIONS_ENABLED

	#define EP_ASSERT_IMPL(expression, message) \
		if (expression) { } \
		else \
		{ \
			EP_FATAL("Assertion failed!  {}\nExpression: {}\nFile: {}\nLine: {}", \
				message, #expression, __FILE__, __LINE__); \
			EP_DEBUGBREAK(); \
			throw Enterprise::Exceptions::AssertFailed(); \
		}
#elif EP_CONFIG_DEV
	#define EP_ASSERTIONS_ENABLED

	#ifdef EP_SCOPE_CORE
		#define EP_ASSERT_IMPL(expression, message) \
			if (expression) { } \
			else \
			{ \
				std::wstringstream messagestream; \
				messagestream << "Assertion failed!  " << message << \
					"\nExpression: " << #expression << "\nFile: " << __FILE__ << "\nLine: " << __LINE__ << \
					"\n\nThe Enterprise engine library has encountered a condition that " \
					"should never occur.  It has terminated the application."; \
				Enterprise::Platform::DisplayErrorDialog(messagestream.str().c_str()); \
				EP_DEBUGBREAK(); \
				throw Enterprise::Exceptions::AssertFailed(); \
			}
	#else
		#define EP_ASSERT_IMPL(expression, message) \
			if (expression) { } \
			else \
			{ \
				std::wstringstream messagestream; \
				messagestream << "Assertion failed!  " << message << \
					"\nExpression: " << #expression << "\nFile: " << __FILE__ << "\nLine: " << __LINE__ << \
					"\n\nThis Enterprise application has encountered a condition that " \
					"should never occur.  It has terminated itself."; \
				Enterprise::Platform::DisplayErrorDialog(messagestream.str().c_str()); \
				EP_DEBUGBREAK(); \
				throw Enterprise::Exceptions::AssertFailed(); \
			}
	#endif

#endif


#ifdef EP_ASSERTIONS_ENABLED
	/// Assert that the expression is true.  Stripped out of Dist builds.
	#define EP_ASSERT(expression)					EP_ASSERT_IMPL(expression, "")
	/// Assert that the expression is true, and print a debug message.  Stripped out of Dist builds.
	#define EP_ASSERTF(expression, message)			EP_ASSERT_IMPL(expression, message)
	/// Assert that the expression is true.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFY(expression)					EP_ASSERT_IMPL(expression, "")
	/// Assert that the expression is true, and print a debug message.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFYF(expression, message)			EP_ASSERT_IMPL(expression, message)
#else
	/// Assert that the expression is true.  Stripped out of Dist builds.
	#define EP_ASSERT(expression)
	/// Assert that the expression is true, and print a debug message.  Stripped out of Dist builds.
	#define EP_ASSERTF(expression, message)
	/// Assert that the expression is true.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFY(expression)					expression
	/// Assert that the expression is true, and print a debug message.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFYF(expression, message)			expression
#endif


#ifdef EP_SLOWASSERTIONS_ENABLED
	/// Assert that the expression is true.  Stripped out of Dev and Dist builds.
	#define EP_ASSERT_SLOW(expression)				EP_ASSERT_IMPL(expression, "")
	/// Assert that the expression is true, and print a debug message.  Stripped out of Dev and Dist builds.
	#define EP_ASSERTF_SLOW(expression, message)	EP_ASSERT_IMPL(expression, message)
	/// Assert that the expression is true.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFY_SLOW(expression)				EP_ASSERT_IMPL(expression, "")
	/// Assert that the expression is true, and print a debug message.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFYF_SLOW(expression, message)	EP_ASSERT_IMPL(expression, message)
#else
	/// Assert that the expression is true.  Stripped out of Dev and Dist builds.
	#define EP_ASSERT_SLOW(expression)	
	/// Assert that the expression is true, and print a debug message.  Stripped out of Dev and Dist builds.
	#define EP_ASSERTF_SLOW(expression, message)
	/// Assert that the expression is true.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFY_SLOW(expression)				expression
	/// Assert that the expression is true, and print a debug message.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFYF_SLOW(expression, message)	expression
#endif

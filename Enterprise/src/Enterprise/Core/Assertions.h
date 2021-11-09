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

#ifndef EP_CONFIG_DIST
#ifdef _WIN32
/// Portably triggers a breakpoint in the debugger.
#define EP_DEBUGBREAK() (__nop(), __debugbreak())
#elif defined(__APPLE__) && defined(__MACH__)
/// Portably triggers a breakpoint in the debugger.
#define EP_DEBUGBREAK() __asm__("int $3")
#endif
#else
#define EP_DEBUGBREAK() 
#endif


/// @cond DOXYGEN_SKIP

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
				/*EP_DEBUGBREAK();*/ \
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
				/*EP_DEBUGBREAK();*/ \
				throw Enterprise::Exceptions::AssertFailed(); \
			}
	#endif
#endif

/// @endcond


#ifdef EP_ASSERTIONS_ENABLED
	/// Assert that an expression is true.  The expression is stripped out of Dist builds.
	#define EP_ASSERT(expression)										EP_ASSERT_IMPL(expression, "")
	/// Assert that an expression is true, and log an error message if it's not.  The expression is stripped out of Dist builds.
	#define EP_ASSERTF(expression, message)								EP_ASSERT_IMPL(expression, message)
	/// Assert that an expression is true.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFY(expression)										EP_ASSERT_IMPL(expression, "")
	/// Assert that an expression is true, and log an error message if it's not.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFYF(expression, message)								EP_ASSERT_IMPL(expression, message)
	/// Assert that an expression is equal to something.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFY_EQ(expression, compareexpr)						EP_ASSERT_IMPL(expression == compareexpr, "")
	/// Assert that an expression is equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFYF_EQ(expression, compareexpr, message)				EP_ASSERT_IMPL(expression == compareexpr, message)
	/// Assert that an expression is not equal to something.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFY_NEQ(expression, compareexpr)						EP_ASSERT_IMPL(expression != compareexpr, "")
	/// Assert that an expression is not equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFYF_NEQ(expression, compareexpr, message)			EP_ASSERT_IMPL(expression != compareexpr, message)

	/// Assert that a code path is never reached.
	#define EP_ASSERT_NOENTRY() 										EP_ASSERTF(false, "Call to EP_ASSERT_NOENTRY().")
	/// Assert that a code path is only executed once.
	#define EP_ASSERT_NOREENTRY() \
	{ \
		static bool s_linereached##__LINE__ = false; \
		EP_ASSERTF(!s_linereached##__LINE__, "Second call to EP_ASSERT_NOREENTRY()."); \
		s_linereached##__LINE__ = true; \
	}
	/// Execute code in assertion scenarios.  The expression is stripped out of Dist builds.
	/// @warning Do not call code with side-effects in this code block, as it is stripped from distribution builds.
	#define EP_ASSERT_CODE(code)										do { code; } while ( false );
#else
	/// Assert that an expression is true.  The expression is stripped out of Dist builds.
	#define EP_ASSERT(expression)
	/// Assert that an expression is true, and log an error message if it's not.  The expression is stripped out of Dist builds.
	#define EP_ASSERTF(expression, message)
	/// Assert that an expression is true.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFY(expression)										expression;
	/// Assert that an expression is true, and log an error message if it's not.  Evaluates to just the expression in Dist builds.
	#define EP_VERIFYF(expression, message)								expression;
	/// Assert that an expression is equal to something.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFY_EQ(expression, compareexpr)						expression;
	/// Assert that an expression is equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFYF_EQ(expression, compareexpr, message)				expression;
	/// Assert that an expression is not equal to something.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFY_NEQ(expression, compareexpr)						expression;
	/// Assert that an expression is not equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dist builds.
	#define EP_VERIFYF_NEQ(expression, compareexpr, message)			expression;

	/// Assert that a code path is never reached.
	#define EP_ASSERT_NOENTRY()
	/// Assert that a code path is only executed once.
	#define EP_ASSERT_NOREENTRY()
	/// Execute code in assertion scenarios.  The code is stripped out of Dist builds.
	/// @warning Do not call code with side-effects in this code block.
	#define EP_ASSERT_CODE(code)
#endif


#ifdef EP_SLOWASSERTIONS_ENABLED
	/// Assert that an expression is true.  The expression is stripped out of Dev and Dist builds.
	#define EP_ASSERT_SLOW(expression)									EP_ASSERT_IMPL(expression, "")
	/// Assert that an expression is true, and log an error message if it's not.  The expression is stripped out of Dev and Dist builds.
	#define EP_ASSERTF_SLOW(expression, message)						EP_ASSERT_IMPL(expression, message)
	/// Assert that an expression is true.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFY_SLOW(expression)									EP_ASSERT_IMPL(expression, "")
	/// Assert that an expression is true, and log an error message if it's not.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFYF_SLOW(expression, message)						EP_ASSERT_IMPL(expression, message)
	/// Assert that an expression is equal to something.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFY_EQ_SLOW(expression, compareexpr)					EP_ASSERT_IMPL(expression == compareexpr, "")
	/// Assert that an expression is equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFYF_EQ_SLOW(expression, compareexpr, message)		EP_ASSERT_IMPL(expression == compareexpr, message)
	/// Assert that an expression is not equal to something.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFY_NEQ_SLOW(expression, compareexpr)					EP_ASSERT_IMPL(expression != compareexpr, "")
	/// Assert that an expression is not equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFYF_NEQ_SLOW(expression, compareexpr, message)		EP_ASSERT_IMPL(expression != compareexpr, message)
	/// Execute code in assertion scenarios.  The expression is stripped out of Dev and Dist builds.
	/// @warning Do not call code with side-effects in this code block, as it is stripped from distribution builds.
	#define EP_ASSERT_CODE_SLOW(code)									do { code; } while ( false );
#else
	/// Assert that an expression is true.  The expression is stripped out of Dev and Dist builds.
	#define EP_ASSERT_SLOW(expression)
	/// Assert that an expression is true, and log an error message if it's not.  The expression is stripped out of Dev and Dist builds.
	#define EP_ASSERTF_SLOW(expression, message)
	/// Assert that an expression is true.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFY_SLOW(expression)									expression;
	/// Assert that an expression is true, and log an error message if it's not.  Evaluates to just the expression in Dev and Dist builds.
	#define EP_VERIFYF_SLOW(expression, message)						expression;
	/// Assert that an expression is equal to something.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFY_EQ_SLOW(expression, compareexpr)					expression;
	/// Assert that an expression is equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFYF_EQ_SLOW(expression, compareexpr, message)		expression;
	/// Assert that an expression is not equal to something.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFY_NEQ_SLOW(expression, compareexpr)					expression;
	/// Assert that an expression is not equal to something, and log an error message if it's not.  Evaluates to just the first expression in Dev and Dist builds.
	#define EP_VERIFYF_NEQ_SLOW(expression, compareexpr, message)		expression;
	/// Execute code in assertion scenarios.  The code is stripped out of Dev and Dist builds.
	/// @warning Do not call code with side-effects in this code block.
	#define EP_ASSERT_CODE_SLOW(code)
#endif

#pragma once

/* Core.h
	This file contains engine-wide defines, classes, and macros.  This should have zero dependencies and be safe to include
	anywhere.
*/

// Assertion macros

#ifdef EP_CONFIG_DEBUG // Break on line, log to console
#ifdef EP_SCOPE_CORE
#define ASSERT(condition)  if(!(condition)) \
	{ \
		EP_FATAL("Assertion failed: {}\n{}, line {}\n\
The Enterprise library has encountered a condition which should never occur and has requested that the application terminate.",\
#condition, __FILE__, __LINE__); __debugbreak(); /*TODO: Handle breakpoint in platform-agnostic way*/ \
	}
#else
#define ASSERT(condition)  if(!(condition)) \
	{ \
		EP_FATAL("Assertion failed: {}\n{}, line {}\n\
The game has encountered a condition which should never occur and has self-terminated.",\
#condition, __FILE__, __LINE__); __debugbreak(); /*TODO: Handle breakpoint in platform-agnostic way*/ \
	}
#endif
#elif EP_CONFIG_RELEASE // Terminate application, produce pop-up message
#ifdef EP_SCOPE_CORE
#define ASSERT(condition)  if(!(condition)) \
	{ \
		EP_FATAL("Assertion failed: {}\n{}, line {}\n\
The Enterprise library has encountered a condition which should never occur and has requested that the application terminate.",\
#condition, __FILE__, __LINE__); /*TODO: Implement error pop-up*/ \
	}
#else
#define ASSERT(condition)  if(!(condition)) \
	{ \
		EP_FATAL("Assertion failed: {}\n{}, line {}\n\
The game has encountered a condition which should never occur and has self-terminated.",\
#condition, __FILE__, __LINE__); /*TODO: Implement error pop-up*/ \
	}
#endif
#elif EP_CONFIG_DIST // Stripped from distribution
#define ASSERT(condition) 
#endif

// Quickly assemble bit fields
#define BIT(x) (1 << x)
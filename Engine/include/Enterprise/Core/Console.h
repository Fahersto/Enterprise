#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Enterprise 
{

/// The Enterprise debug console.
/// @details Use this class to log debug messages during runtime.
/// @note		This version of the console may be depreciated outright after a conventional drop-down
///				developer's console is implemented.
class Console
{
public:

	/// Logs a message to the debug console.
	/// @tparam ...Args Types of the format string arguments.
	/// @param level The desired logging level.
	/// @param fmt Format string.  Use curly brackets to specify replacement fields.
	/// @param ...args Format string arguments.
	/// @warning	Do not call this method directly.  Instead, use EP_TRACE, EP_DEBUG, EP_INFO, EP_WARN, EP_ERROR,
	///				or EP_FATAL to log to the console.  This ensures that the logging code is properly stripped from
	///				Dev and Release builds.
	template<typename... Args>
	inline static void Log(spdlog::level::level_enum level, spdlog::string_view_t fmt, const Args&... args) 
	{
		#ifndef EP_CONFIG_DEBUG
		// Log should not be called in Dev or Release builds.
		// Ensure you're using EP_TRACE, EP_DEBUG, EP_INFO, EP_WARN, EP_ERROR, or EP_FATAL to log to the console.
		EP_ASSERT(false);
		#endif

		#ifdef EP_BUILD_CORE
		s_CoreLogger->log(level, fmt, args...);
		#else
		s_ClientLogger->log(level, fmt, args...);
		#endif
	}
	/// Logs a message to the debug console.
	/// @tparam T Type of the message argument.
	/// @param level The desired logging level.
	/// @param msg A loggable object reference.
	/// @pre @p msg must have overloaded the \<\< operator for output streams.
	/// @warning	Do not call this method directly.  Instead, use EP_TRACE, EP_DEBUG, EP_INFO, EP_WARN, EP_ERROR,
	///				or EP_FATAL to log to the console.  This ensures that the logging code is properly stripped from
	///				Dev and Release builds.
	template<typename T>
	inline static void Log(spdlog::level::level_enum level, const T& msg) 
	{
		#ifndef EP_CONFIG_DEBUG
		// Log should not be called in Dev or Release builds.
		// Ensure you're using EP_TRACE, EP_DEBUG, EP_INFO, EP_WARN, EP_ERROR, or EP_FATAL to log to the console.
		EP_ASSERT(false);
		#endif

		#ifdef EP_BUILD_CORE
		s_CoreLogger->log(level, msg);
		#else
		s_ClientLogger->log(level, msg);
		#endif
	}

private:
	// Needed for access to Init() and Cleanup().
	friend class Application;

	/// The spdlog logger object used in the engine library.
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	/// The spdlog logger object used in the client application.
	EP_API static std::shared_ptr<spdlog::logger> s_ClientLogger;

	/// Sets up the debug console.
	/// @note This is defined per platform.
	static void Init();

	/// Cleans up the debug console.
	/// @note This is defined per platform.
	static void Cleanup();

	/// Initializes the spdlog logger objects.
	static void InitSpdlog() 
	{
		spdlog::set_pattern("%6n %^[%T]: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("CORE");
		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
		s_ClientLogger->set_level(spdlog::level::trace);
	};
};

}

// Logging Macros

#ifdef EP_CONFIG_DEBUG

	/// Logs a message to the debug console at the lowest logging level.
	#define EP_TRACE(...)	Enterprise::Console::Log(spdlog::level::level_enum::trace, __VA_ARGS__)
	/// Logs a message to the debug console.
	#define EP_DEBUG(...)	Enterprise::Console::Log(spdlog::level::level_enum::debug, __VA_ARGS__)
	/// Logs an informational message to the debug console.
	#define EP_INFO(...)	Enterprise::Console::Log(spdlog::level::level_enum::info, __VA_ARGS__)
	/// Logs a warning to the debug console.
	#define EP_WARN(...)	Enterprise::Console::Log(spdlog::level::level_enum::warn, __VA_ARGS__)
	/// Logs a non-fatal error message to the debug console.
	#define EP_ERROR(...)	Enterprise::Console::Log(spdlog::level::level_enum::err, __VA_ARGS__)
	/// Logs a fatal error message to the debug console.
	#define EP_FATAL(...)	Enterprise::Console::Log(spdlog::level::level_enum::critical, __VA_ARGS__)

#else // Strip from Dev and Release builds

	/// Logs a message to the debug console at the lowest logging level.
	#define EP_TRACE(...)
	/// Logs a message to the debug console.
	#define EP_DEBUG(...)
	/// Logs an informational message to the debug console.
	#define EP_INFO(...)
	/// Logs a warning to the debug console.
	#define EP_WARN(...)
	/// Logs a non-fatal error message to the debug console.
	#define EP_ERROR(...)
	/// Logs a fatal error message to the debug console.
	#define EP_FATAL(...)

#endif

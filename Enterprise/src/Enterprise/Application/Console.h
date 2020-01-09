#pragma once

// Spdlog includes (should be portable)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Enterprise {

	class Console
	{
	public:
		static void Init();
		static void Cleanup();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Logging Macros
#ifdef EP_CONFIG_DEBUG
	#ifdef EP_SCOPE_CORE // Core
	#define EP_TRACE(...)	::Enterprise::Console::GetCoreLogger()->trace(__VA_ARGS__)
	#define EP_DEBUG(...)	::Enterprise::Console::GetCoreLogger()->debug(__VA_ARGS__)
	#define EP_INFO(...)	::Enterprise::Console::GetCoreLogger()->info(__VA_ARGS__)
	#define EP_WARN(...)	::Enterprise::Console::GetCoreLogger()->warn(__VA_ARGS__)
	#define EP_ERROR(...)	::Enterprise::Console::GetCoreLogger()->error(__VA_ARGS__)
	#define EP_FATAL(...)	::Enterprise::Console::GetCoreLogger()->critical(__VA_ARGS__)
	#elif EP_SCOPE_CLIENT // Client
	#define EP_TRACE(...)	::Enterprise::Console::GetClientLogger()->trace(__VA_ARGS__)
	#define EP_DEBUG(...)	::Enterprise::Console::GetClientLogger()->debug(__VA_ARGS__)
	#define EP_INFO(...)	::Enterprise::Console::GetClientLogger()->info(__VA_ARGS__)
	#define EP_WARN(...)	::Enterprise::Console::GetClientLogger()->warn(__VA_ARGS__)
	#define EP_ERROR(...)	::Enterprise::Console::GetClientLogger()->error(__VA_ARGS__)
	#define EP_FATAL(...)	::Enterprise::Console::GetClientLogger()->critical(__VA_ARGS__)
	#endif
#else // Strip from release builds
	#define EP_TRACE(...)
	#define EP_DEBUG(...)
	#define EP_INFO(...)
	#define EP_WARN(...)
	#define EP_ERROR(...)
	#define EP_FATAL(...)
#endif
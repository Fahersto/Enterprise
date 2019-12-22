#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Enterprise {


	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Log Macros
#ifdef EP_DEBUG
	#ifdef EP_CORESCOPE // Core
	#define EP_TRACE(...)	::Enterprise::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define EP_ERROR(...)	::Enterprise::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define EP_INFO(...)	::Enterprise::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define EP_WARN(...)	::Enterprise::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define EP_FATAL(...)	::Enterprise::Log::GetCoreLogger()->critical(__VA_ARGS__)
	#elif EP_CLIENTSCOPE // Client
	#define EP_TRACE(...)	::Enterprise::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define EP_ERROR(...)	::Enterprise::Log::GetClientLogger()->error(__VA_ARGS__)
	#define EP_INFO(...)	::Enterprise::Log::GetClientLogger()->info(__VA_ARGS__)
	#define EP_WARN(...)	::Enterprise::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define EP_FATAL(...)	::Enterprise::Log::GetClientLogger()->critical(__VA_ARGS__)
	#endif
#else // Strip from release builds
	#define EP_TRACE(...)
	#define EP_ERROR(...)
	#define EP_INFO(...)
	#define EP_WARN(...)
	#define EP_FATAL(...)
#endif
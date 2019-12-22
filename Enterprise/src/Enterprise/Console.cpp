#include "EP_PCH.h"
#include "Console.h"

namespace Enterprise
{
	std::shared_ptr<spdlog::logger> Console::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Console::s_ClientLogger;

	void Console::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("CORE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}
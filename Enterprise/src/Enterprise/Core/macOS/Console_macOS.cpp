#include "EP_PCH.h"
#ifdef EP_PLATFORM_MACOS

#include "Core.h"
#include "Enterprise/Core/Console.h"

// Static definitions
std::shared_ptr<spdlog::logger> Enterprise::Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Enterprise::Console::s_ClientLogger;

void Enterprise::Console::Init()
{
    InitLoggers();  // spdlog
}

void Enterprise::Console::Cleanup() {}

#endif

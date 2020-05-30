#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

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

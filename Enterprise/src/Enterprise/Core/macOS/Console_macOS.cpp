#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Core.h"
#include "Enterprise/Core/Console.h"

namespace Enterprise
{

// Static definitions
std::shared_ptr<spdlog::logger> Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Console::s_ClientLogger;

void Console::Init()
{
    InitSpdlog();  // spdlog
}

void Console::Cleanup()
{
    spdlog::shutdown();
}

}

#endif

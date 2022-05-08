#pragma once
#ifdef EP_BUILD_DYNAMIC

#include "Enterprise/Core/SharedLibraryExports.h"
#include <string>

namespace Enterprise
{

bool EP_API LoadGameModule(const std::string& projectFilePath);
void EP_API UnloadGameModule();

}

#endif // EP_BUILD_DYNAMIC

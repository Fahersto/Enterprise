#pragma once
#ifdef _WIN32
#include "Enterprise/Core/SharedLibraryExports.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

std::string EP_API WCHARtoUTF8(const WCHAR* str);
std::wstring EP_API UTF8toWCHAR(const std::string& str);

std::string EP_API Win32_LastErrorMsg();

#endif // _WIN32

#pragma once
#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::string WCHARtoUTF8(const WCHAR* str);

std::wstring UTF8toWCHAR(const std::string& str);

std::string Win32_LastErrorMsg();

#endif // _WIN32

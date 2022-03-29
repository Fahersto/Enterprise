#include "Enterprise/Core/Win32APIHelpers.h"

#ifdef _WIN32

std::string WCHARtoUTF8(const WCHAR* str)
{
	size_t widestring_len = wcslen(str);

	if (widestring_len > 0)
	{
		std::string returnVal(WideCharToMultiByte(CP_UTF8, 0, str, widestring_len, NULL, 0, NULL, NULL), 0);
		WideCharToMultiByte(CP_UTF8, 0, str, widestring_len, &returnVal[0], returnVal.size(), NULL, NULL);
		return returnVal;
	}
	else
	{
		return std::string();
	}
}

std::wstring UTF8toWCHAR(const std::string& str)
{
	if (str.length() > 0)
	{
		std::wstring returnVal(MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0), 0);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, returnVal.data(), returnVal.size());
		return returnVal;
	}
	else
	{
		return std::wstring();
	}
}

std::string Win32_LastErrorMsg()
{
	DWORD errorcode = GetLastError();

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	return std::to_string(errorcode) + ", " + WCHARtoUTF8((LPTSTR)lpMsgBuf);
}

#endif // _WIN32

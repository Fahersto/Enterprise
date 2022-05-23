#ifdef _WIN32
#include <windows.h>
#include "Enterprise/Core/ErrorMessageBox.h"
#include "Enterprise/Core/Win32APIHelpers.h"

namespace Enterprise::Platform
{

void DisplayErrorDialog(const char* ErrorMessage)
{
	DisplayErrorDialog(ErrorMessage);
}

void DisplayErrorDialog(std::string& ErrorMessage)
{
	std::wstring errorMessage_wide = UTF8toWCHAR(ErrorMessage);

	MessageBox(
		NULL,
		errorMessage_wide.c_str(),
		(LPCWSTR)L"Enterprise Error",
		MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
}

}

#endif // _WIN32

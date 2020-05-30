#ifdef EP_PLATFORM_WINDOWS

#include "EP_PCH.h"
#include "Enterprise/Core/ErrorMessageBox.h"

void Enterprise::Platform::DisplayErrorDialog(std::wstring& ErrorMessage)
{
    DisplayErrorDialog(ErrorMessage.c_str());
}

void Enterprise::Platform::DisplayErrorDialog(const wchar_t* ErrorMessage)
{
    MessageBox(
               NULL,
               ErrorMessage,
               (LPCWSTR)L"Enterprise Error",
               MB_ICONERROR | MB_OK | MB_DEFBUTTON1
               );
}

#endif

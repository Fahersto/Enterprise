#include "EP_PCH.h"
#ifdef _WIN32

#include "Enterprise/Core/ErrorMessageBox.h"

namespace Enterprise::Platform
{

void DisplayErrorDialog(std::wstring& ErrorMessage)
{
    DisplayErrorDialog(ErrorMessage.c_str());
}

void DisplayErrorDialog(const wchar_t* ErrorMessage)
{
    MessageBox(
               NULL,
               ErrorMessage,
               (LPCWSTR)L"Enterprise Error",
               MB_ICONERROR | MB_OK | MB_DEFBUTTON1
               );
}

}

#endif

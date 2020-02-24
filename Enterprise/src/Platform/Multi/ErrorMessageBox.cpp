#include "EP_PCH.h"
#include "ErrorMessageBox.h"

#ifdef EP_PLATFORM_WINDOWS
void Enterprise::Platform::DisplayErrorDialog(std::wstring& ErrorMessage)
	{
		MessageBox(
			NULL,
			(LPCWSTR)ErrorMessage.c_str(),
			(LPCWSTR)L"Enterprise Error",
			MB_ICONERROR | MB_OK | MB_DEFBUTTON1
		);
	}
#endif
//TODO: Add error pop-up for other platforms
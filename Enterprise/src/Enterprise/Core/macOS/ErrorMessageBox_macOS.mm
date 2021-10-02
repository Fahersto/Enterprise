#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/Core/ErrorMessageBox.h"

namespace Enterprise::Platform
{

void DisplayErrorDialog(std::wstring& ErrorMessage)
{
    DisplayErrorDialog(ErrorMessage.c_str());
}

void DisplayErrorDialog(const wchar_t* ErrorMessage)
{
	@autoreleasepool {
		NSString* convertedErrorMessage = [[[NSString alloc] initWithBytes:ErrorMessage
																	length: (sizeof(wchar_t) * wcslen(ErrorMessage))
																  encoding:NSUTF32LittleEndianStringEncoding] autorelease];
        NSAlert *errorBox = [[[NSAlert alloc] init] autorelease];
        [errorBox setMessageText:@"Enterprise Error"];
        [errorBox setInformativeText:convertedErrorMessage];
        [errorBox addButtonWithTitle:@"Ok"];
        [errorBox setAlertStyle:NSAlertStyleCritical];
        [errorBox runModal];
    }
}

}

#endif

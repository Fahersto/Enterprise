#ifdef EP_PLATFORM_MACOS

#include "EP_PCH.h"
#include "Enterprise/Core/ErrorMessageBox.h"

void Enterprise::Platform::DisplayErrorDialog(std::wstring& ErrorMessage)
{
    DisplayErrorDialog(ErrorMessage.c_str());
}

void Enterprise::Platform::DisplayErrorDialog(const wchar_t* ErrorMessage)
{
    @autoreleasepool {
        NSString* convertedErrorMessage = [[NSString alloc] initWithBytes:ErrorMessage
                                                                   length: (sizeof(wchar_t) * wcslen(ErrorMessage))
                                                                 encoding:NSUTF32LittleEndianStringEncoding];
        NSAlert *errorBox = [[NSAlert alloc] init];
        [errorBox setMessageText:@"Enterprise Error"];
        [errorBox setInformativeText:convertedErrorMessage];
        [errorBox addButtonWithTitle:@"Ok"];
        [errorBox setAlertStyle:NSAlertStyleCritical];
        [errorBox runModal];
    }
}

#endif

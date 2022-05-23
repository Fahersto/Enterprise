#if defined(__APPLE__) && defined(__MACH__)
#import <AppKit/AppKit.h>
#include "Enterprise/Core/ErrorMessageBox.h"

namespace Enterprise::Platform
{

void DisplayErrorDialog(std::string& ErrorMessage)
{
	DisplayErrorDialog(ErrorMessage.c_str());
}

void DisplayErrorDialog(const char* ErrorMessage)
{
	@autoreleasepool {
		NSString* convertedErrorMessage = [[[NSString alloc] initWithBytes:ErrorMessage
																	length:(sizeof(char) * strlen(ErrorMessage))
																  encoding:NSUTF8StringEncoding] autorelease];
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

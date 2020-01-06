#pragma once

/* EntryPoint.h
	This file Includes the main function for games made in Enterprise.  Currently, Enterprise only supports Windows, but
	when support is added for MacOS or other platforms, preprocessor statements here will define which main function is
	included based on configuration settings.
*/


// Creates and returns the client Application.  Defined in client.
extern Enterprise::Application* Enterprise::CreateApplication();

// Include the appropriate Main function.
#ifdef EP_PLATFORM_WINDOWS
#include "Platform/Windows/WinMain.h"
#endif

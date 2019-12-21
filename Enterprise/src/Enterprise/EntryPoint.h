#pragma once

/* EntryPoint.h
	This file Includes the main function for games made in Enterprise.  Currently, Enterprise only supports Windows, but
	when support is added for MacOS or other platforms, preprocessor statements here will define which main function is
	included based on configuration settings.
*/


// Creates and returns the client Application.
extern Enterprise::Application* Enterprise::CreateApplication();

//#ifdef EP_PLATFORM_WINDOWS
//#include "Platform/Windows/WinMain.h"
//// TODO: Include the main functions for other platforms here.
//#endif
//

#include <stdio.h>

int main()
{
	printf("Test\n");
	return 0;
}
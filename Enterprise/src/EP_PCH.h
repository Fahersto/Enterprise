#pragma once
/* EP_PCH.h
	This is the precompiled header file for the entire Enterprise engine.  Include all boilerplate headers here.
*/

#include <memory>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#ifdef EP_PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include <Xinput.h>
#endif
#pragma once
/* EP_PCH.h
	This is the precompiled header file for the entire Enterprise engine.  Include all boilerplate headers here.
*/

#include <memory>
#include <array>
#include <iostream>
#include <fstream>
#include <tuple>
#include <list>

#include <string>
#include <sstream>
#include <functional>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include <exception>

#ifdef EP_PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include <Xinput.h>
#endif
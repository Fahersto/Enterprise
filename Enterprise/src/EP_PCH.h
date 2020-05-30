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
#include <wchar.h>
#include <sstream>
#include <functional>
#include <map>

#include <stdio.h>
#include <fcntl.h>

#include <exception>
#include <csignal>

#ifdef _WIN32
    #include <windows.h>
    #include <windowsx.h>
    #include <Xinput.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
    #ifdef __OBJC__
        #import <Foundation/Foundation.h>
        #import <Cocoa/Cocoa.h>
        #import <AppKit/AppKit.h>
    #endif
#include <mach/mach_time.h>
#endif

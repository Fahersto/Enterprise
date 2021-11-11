#pragma once

// EP_PCH.h
// Enterprise's precompiled header.

#ifdef _WIN32
    #define NOMINMAX // for std::min() and std::max()
#endif

#include <iostream>
#include <fstream>
#include <filesystem>

#include <memory>
#include <array>
#include <tuple>
#include <list>
#include <forward_list>
#include <algorithm>
#include <set>
#include <stack>
#include <variant>

#include <string>
#include <wchar.h>
#include <sstream>
#include "prettyprint.hpp"

#include <functional>
#include <map>

#include <stdio.h>
#include <fcntl.h>

#include <exception>
#include <csignal>

/// @cond DOXYGEN_SKIP
#define _USE_MATH_DEFINES
/// @endcond
#include <math.h>

#include <CTSpookyHash/CTSpooky.h>
#include <HotConsts/HotConsts.h>
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <yaml-cpp/yaml.h>
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#ifdef _WIN32
    #include <windows.h>
    #include <windowsx.h>
    #include <Xinput.h>
    #include <ShlObj.h>
#endif // Win32

#if defined(__APPLE__) && defined(__MACH__)
	#define GL_SILENCE_DEPRECATION
    #ifdef __OBJC__
        #import <AppKit/AppKit.h>
        #import <GameController/GameController.h>
    #endif
    #include <mach/mach_time.h>
	#include <Carbon/Carbon.h> // Used for keycodes in Input_macOS.mm
#endif // macOS

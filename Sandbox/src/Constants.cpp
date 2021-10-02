#include <Enterprise.h>

// Constants.cpp
// A "tweak" file for engine constants.
// Hot constants (defined with the HC macro) are reloaded when this file is saved.

namespace Enterprise::Constants
{

// Application constants
const char* DeveloperName = "Michael Martz";
const char* AppName = "Sandbox";
const char* Version = "0.0.0";
const wchar_t* WindowTitle = L"Enterprise Window Title";

// Time constants
HC(float, PhysFrameLength) = 1.0f / 60.0f;
HC(float, PhysFrameRepeatCap) = 60.0f;

// Graphics constants
const unsigned int TEMP_WindowWidth = 1280;
const unsigned int TEMP_WindowHeight = 720;

}

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
namespace Time
{
HC(double, FixedTimestep) = 1.0 / 240.0;
HC(double, MaxFrameDelta) = 1.0 / 15.0;
}

// Graphics constants
const unsigned int TEMP_WindowWidth = 1280;
const unsigned int TEMP_WindowHeight = 720;

}

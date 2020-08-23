#include <Enterprise.h>

// Constants.cpp
// A "tweak" file for engine constants.
// Hot constants (defined with the HC macro) are reloaded when this file is saved.

namespace Enterprise::Constants
{

// Time constants
HC(float, PhysFrameLength) = 1.0f / 60.0f;
HC(float, PhysFrameRepeatCap) = 60.0f;

}

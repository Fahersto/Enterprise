#include <Enterprise.h>

// Constants.cpp
// This is a sort of "tweak file" for constant values used by the engine.
// This will eventually be moved to a true tweak file which can be loaded at runtime
// (at least, for debug builds).

// Time constants
const float Enterprise::Constants::PhysFrameLength = 1.0f / 50.0f;
const float Enterprise::Constants::PhysFrameRepeatCap = 60.0f;
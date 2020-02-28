#include "EP_PCH.h"
#include "Enterprise/Time/Time.h"

// Static member variables
float Enterprise::Time::m_deltaTime, Enterprise::Time::m_simPhase;

// Time getters
float Enterprise::Time::DeltaTime() { return m_deltaTime; }
float Enterprise::Time::SimPhase() { return m_simPhase; }

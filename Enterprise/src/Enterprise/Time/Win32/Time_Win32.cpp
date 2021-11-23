#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "../Time.h"

using Enterprise::Time;

static LARGE_INTEGER tickFrequency, startTicks, currentTicks;

void Time::PlatformInit()
{
	QueryPerformanceFrequency(&tickFrequency);
	QueryPerformanceCounter(&startTicks);
}

uint64_t Time::GetRawTicks()
{
	QueryPerformanceCounter(&currentTicks);
	return currentTicks.QuadPart - startTicks.QuadPart;
}

uint64_t Time::SecondsToTicks(double seconds)
{
	return uint64_t(seconds * tickFrequency.QuadPart);
}

float Time::TicksToSeconds(uint64_t ticks)
{
	return float(double(ticks) / double(tickFrequency.QuadPart));
}

#endif

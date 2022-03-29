#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
#include "Enterprise/Core.h"
#include "Enterprise/Time/Time.h"

using Enterprise::Time;

static uint64_t startTicks, currentTicks;
static mach_timebase_info_data_t timebase = { 0 };

void Time::PlatformInit()
{
    EP_VERIFY_EQ(mach_timebase_info(&timebase), KERN_SUCCESS);
	startTicks = mach_absolute_time();
}

uint64_t Time::GetRawTicks()
{
	currentTicks = mach_absolute_time();
	return currentTicks - startTicks;
}

uint64_t Time::SecondsToTicks(double seconds)
{
	return seconds * (double)timebase.denom / (double)timebase.numer / 1e-9;
}

float Time::TicksToSeconds(uint64_t ticks)
{
	return (double)ticks * (double)timebase.numer * 1e-9 / (double)timebase.denom;
}

#endif

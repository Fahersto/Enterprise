#ifdef EP_PLATFORM_MACOS

#include "EP_PCH.h"
#include "Core.h"
#include "Enterprise/Time/Time.h"

uint64_t _startCount, _currentCount = {};
double _tickPeriod = {};

void Enterprise::Time::Init()
{
    // Calculate the tick length in seconds
    mach_timebase_info_data_t timebase = { 0 };
    EP_ASSERT(mach_timebase_info(&timebase) == KERN_SUCCESS);
    _tickPeriod = 1e-9 * (double)timebase.numer / (double)timebase.denom;

    // Track what time the program started
    _startCount = mach_absolute_time();
    _currentCount = _startCount;
}

float Enterprise::Time::GetRawTime()
{
    _currentCount = mach_absolute_time();
    return float((_currentCount - _startCount) * _tickPeriod);
}

#endif

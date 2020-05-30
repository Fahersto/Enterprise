#ifdef EP_PLATFORM_WINDOWS

#include "EP_PCH.h"
#include "Core.h"
#include "Enterprise/Time/Time.h"

LARGE_INTEGER _startCount, _currentCount = {};
double _tickPeriod = {};

void Enterprise::Time::Init() 
{
    // Calculate the tick length in seconds
    LARGE_INTEGER QPF;
    EP_ASSERT(QueryPerformanceFrequency(&QPF));
    _tickPeriod = 1.0 / QPF.QuadPart;

    // Track what time the program started
	EP_ASSERT(QueryPerformanceCounter(&_startCount));
	_currentCount = _startCount;
}

float Enterprise::Time::GetRawTime()
{
	QueryPerformanceCounter(&_currentCount);
	return float((_currentCount.QuadPart - _startCount.QuadPart) * _tickPeriod;
}

#endif

#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Time/Time.h"

LARGE_INTEGER _startCount, _currentCount = {};
double _tickPeriod = {};

void Enterprise::Time::Init() 
{
    // Calculate the tick length in seconds
    LARGE_INTEGER QPF;
    QueryPerformanceFrequency(&QPF);
    _tickPeriod = 1.0 / QPF.QuadPart;

    // Track what time the program started
	QueryPerformanceCounter(&_startCount);
	_currentCount = _startCount;
}

float Enterprise::Time::GetRawTime()
{
	QueryPerformanceCounter(&_currentCount);
	return float((_currentCount.QuadPart - _startCount.QuadPart) * _tickPeriod);
}

#endif

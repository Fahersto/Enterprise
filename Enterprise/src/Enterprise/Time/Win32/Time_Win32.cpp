#include "EP_PCH.h"
#include "Core.h"
#include "Enterprise/Time/Time.h"

/* Time_Win32
	Windows-specific Time functions.
*/
#ifdef EP_PLATFORM_WINDOWS

LARGE_INTEGER QPF, StartCount, CurrentCount{};

void Enterprise::Time::Init() 
{
	// Initialize QPC
	EP_ASSERT(QueryPerformanceFrequency(&QPF));
	EP_ASSERT(QueryPerformanceCounter(&StartCount));
	CurrentCount = StartCount;
}

float Enterprise::Time::GetRawTime()
{
	QueryPerformanceCounter(&CurrentCount);
	return float(CurrentCount.QuadPart - StartCount.QuadPart) / float(QPF.QuadPart);
}
#endif

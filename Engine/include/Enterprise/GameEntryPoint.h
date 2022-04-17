#pragma once

#ifdef _WIN32
	#ifdef EP_BUILD_DYNAMIC
		#define EPGAME_API extern "C" __declspec(dllexport)
	#else // Static
		#define EPGAME_API
	#endif
#else // macOS
	#define EPGAME_API extern "C"
#endif

EPGAME_API void GameInit();
EPGAME_API void GameCleanup();
EPGAME_API void PieInit();
EPGAME_API void PieCleanup();
EPGAME_API void SysInit();
EPGAME_API void SysCleanup();

#pragma once

#ifdef _WIN32
	#ifdef EP_BUILD_DYNAMIC
		#ifdef EP_BUILD_GAME
			#define EPGAME_API extern "C" __declspec(dllexport)
		#elif defined(EP_BUILD_CORE)
			#define EPGAME_API __declspec(dllexport)
		#else // !EP_BUILD_GAME
			#define EPGAME_API
		#endif
	#else
		#define EPGAME_API
	#endif
#else // macOS
	#define EPGAME_API
#endif

#ifdef EP_BUILD_DYNAMIC
extern void (*GameSysInit_Ptr)();
extern void (*GameSysCleanup_Ptr)();
extern void (*GameInit_Ptr)();
extern void (*GameCleanup_Ptr)();
extern void (*PieInit_Ptr)();
extern void (*PieCleanup_Ptr)();
#endif

EPGAME_API void GameSysInit();
EPGAME_API void GameSysCleanup();
EPGAME_API void GameInit();
EPGAME_API void GameCleanup();
EPGAME_API void PieInit();
EPGAME_API void PieCleanup();

#pragma once

// Export symbols from dynamic library builds
#ifdef _WIN32
	#ifdef EP_BUILD_DYNAMIC
		#ifdef EP_BUILD_CORE
			#define EP_API __declspec(dllexport)
		#else
			#define EP_API __declspec(dllimport)
		#endif
	#else // Static
		#define EP_API
	#endif
#else // macOS
	#define EP_API
#endif

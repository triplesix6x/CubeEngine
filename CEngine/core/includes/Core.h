#pragma once

#ifdef CUBE_PLATFORM_WINDOWS
	#ifdef CUBE_BUILD_DLL
		#define CUBE_API __declspec(dllexport)
	#else
		#define CUBE_API __declspec(dllimport)
	#endif
#else
	#error Cube only supports Windows
#endif
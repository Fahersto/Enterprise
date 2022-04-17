#pragma once

// Needed for friending the Win32 entry point
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

// Needed for friending the macOS entry point
#if defined(__APPLE__) && defined (__MACH__)
int main(int argc, const char * argv[]);
#endif

namespace Editor
{

class Runtime
{
public:
	static void Quit() { isRunning = false; };
	static bool Run();

private:
#ifdef _WIN32
	friend int WINAPI ::WinMain(_In_ HINSTANCE hInstance,
								_In_opt_ HINSTANCE hPrevInstance,
								_In_ LPSTR lpCmdLine,
								_In_ int nCmdShow);
#elif defined(__APPLE__) && defined(__MACH__)
	friend int ::main(int argc, const char* argv[]);
#endif

	static bool isRunning;

	Runtime();
	~Runtime();
};

}

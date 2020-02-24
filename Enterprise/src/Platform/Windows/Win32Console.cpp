#include "EP_PCH.h"
#include "Core.h"
#include "Enterprise/Application/Console.h"

#ifdef EP_PLATFORM_WINDOWS

// Static definitions
std::shared_ptr<spdlog::logger> Enterprise::Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Enterprise::Console::s_ClientLogger;

void Enterprise::Console::Init()
{
	// Windows
	EP_ASSERT(AllocConsole());
	//HANDLE h_ConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle(EP_CONSOLE_TITLE);

	// spllog
	InitLoggers();
}

void Enterprise::Console::Cleanup() {
	
	// Display final message
	HANDLE h_ConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD CharsWritten;
	WriteConsole(h_ConsoleOut, L"\nApplication has closed.  Press any key to exit", 47, &CharsWritten, 0);

	// Change input mode to accept single characters
	HANDLE h_ConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(h_ConsoleIn, 0);

	// Read keypress
	TCHAR InputBuffer;
	DWORD CharsRead;
	FlushConsoleInputBuffer(h_ConsoleIn); //Prevents prior keypresses from closing the console
	ReadConsole(h_ConsoleIn, &InputBuffer, 1, &CharsRead, 0);
}

#endif
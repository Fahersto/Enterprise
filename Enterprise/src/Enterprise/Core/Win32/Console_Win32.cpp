#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Core/Console.h"

// Static definitions
std::shared_ptr<spdlog::logger> Enterprise::Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Enterprise::Console::s_ClientLogger;

void Enterprise::Console::Init()
{
	EP_ASSERT(AllocConsole());
	SetConsoleTitle(EP_CONSOLE_TITLE);

	InitLoggers(); // spdlog
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

#include "EP_PCH.h"
#include "Console.h"

// Title of the debug console
#define EP_CONSOLE_TITLE L"Enterprise Debug Console"

namespace Enterprise
{
	std::shared_ptr<spdlog::logger> Console::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Console::s_ClientLogger;

	void Console::Init()
	{
		#ifdef EP_PLATFORM_WINDOWS
		AllocConsole(); // Create console window
		HANDLE h_ConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle(EP_CONSOLE_TITLE); // Set window title
		#endif

		// Set up loggers
		spdlog::set_pattern("%6n %^[%T]: %v%$ (%oms)");
		s_CoreLogger = spdlog::stdout_color_mt("CORE");
		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
		s_ClientLogger->set_level(spdlog::level::trace);
	}


	// Keeps console window open when program terminates, allowing developer to review logs before closing.
	void Console::Cleanup()
	{
		#ifdef EP_PLATFORM_WINDOWS

		// Display prompt
		HANDLE h_ConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD CharsWritten;
		WriteConsole(h_ConsoleOut, L"\nClient has closed.  Press any key to exit", 42, &CharsWritten, 0);

		// Change input mode to accept single characters
		HANDLE h_ConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleMode(h_ConsoleIn, 0);

		// Read keypress
		TCHAR InputBuffer;
		DWORD CharsRead;
		FlushConsoleInputBuffer(h_ConsoleIn); //Prevents earlier keypresses from closing the program
		ReadConsole(h_ConsoleIn, &InputBuffer, 1, &CharsRead, 0);

		#endif
	}
}
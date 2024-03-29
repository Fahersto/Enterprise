#ifdef _WIN32

#include "Enterprise/Core.h"
#include "Enterprise/Core/Console.h"

namespace Enterprise
{

// Static definitions
std::shared_ptr<spdlog::logger> Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Console::s_ClientLogger;

void Console::Init()
{
	//  EP_VERIFY(AllocConsole());
	//  SetConsoleTitle(L"Enterprise Debug Console");

	//  intptr_t lStdHandle;
	//  int hConHandle;
	//  FILE* fp;

	//  // Redirect STDOUT to the console
	//  lStdHandle = (intptr_t)GetStdHandle(STD_OUTPUT_HANDLE);
	//  EP_ASSERT(lStdHandle != (intptr_t)INVALID_HANDLE_VALUE);
	//  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	//  EP_ASSERT(hConHandle != -1);
	//  fp = _fdopen(hConHandle, "w");
	//  EP_ASSERT(fp);
	//  freopen_s(&fp, "CONOUT$", "w", stdout);

	//  // Redirect STDIN to the console
	//  lStdHandle = (intptr_t)GetStdHandle(STD_INPUT_HANDLE);
	//  EP_ASSERT(lStdHandle != (intptr_t)INVALID_HANDLE_VALUE);
	//  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	//  EP_ASSERT(hConHandle != -1);
	//  fp = _fdopen(hConHandle, "r");
	//  EP_ASSERT(fp);
	//  freopen_s(&fp, "CONOUT$", "r", stdout);

	//  // Redirect STDERR to the console
	//  lStdHandle = (intptr_t)GetStdHandle(STD_ERROR_HANDLE);
	//  EP_ASSERT(lStdHandle != (intptr_t)INVALID_HANDLE_VALUE);
	//  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	//  EP_ASSERT(hConHandle != -1);
	//  fp = _fdopen(hConHandle, "w");
	//  EP_ASSERT(fp);
	//  freopen_s(&fp, "CONOUT$", "w", stdout);

	InitSpdlog();
}

void Console::Cleanup() {

	spdlog::shutdown();

	//// Display final message
	//HANDLE h_ConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//DWORD CharsWritten;
	//WriteConsole(h_ConsoleOut, L"\nApplication has closed.  Press any key to exit", 47, &CharsWritten, 0);

	//// Change input mode to accept single characters
	//HANDLE h_ConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	//SetConsoleMode(h_ConsoleIn, 0);

	//// Read keypress
	//TCHAR InputBuffer;
	//DWORD CharsRead;
	//FlushConsoleInputBuffer(h_ConsoleIn); //Prevents prior keypresses from closing the console
	//ReadConsole(h_ConsoleIn, &InputBuffer, 1, &CharsRead, 0);
}

}

#endif // _WIN32

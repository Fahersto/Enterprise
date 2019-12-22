#include "EP_PCH.h"
#include "WinMain.h"

#include "Enterprise/Console.h"
#include "Enterprise/Application.h"

// TODO: Move window configuration to a .ini or something
#define WIN_TITLE "ENTERPRISE PRE-ALPHA"
#define UPDATE_SPEED 60
#define DRAW_SPEED 60
#define WIN_WIDTH 1080
#define WIN_HEIGHT 720

// WinMain:
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// Create Console
	#ifdef EP_DEBUG
	InitializeDebugConsole();
	#endif
	
	// Handle command line arguments
	// TODO: Handle command line arguments
	
	// Create the Application
	auto app = Enterprise::CreateApplication();

	EP_TRACE("Trace from Core");
	EP_WARN("Warn from Core");
	EP_INFO("Info from Core");
	EP_ERROR("Error from Core");
	EP_FATAL("Fatal from Core");

	// TODO: Abstract window creation so it can be handled by Application
	{
		HWND hWnd = CreateClientWindow(hInstance);	// Create the game window and store the handle.
		if (!hWnd)									// Abort if window fails to create.
			return 1;
		ShowWindow(hWnd, nCmdShow);					// Tell Windows to display the window.
	}

	// Set up timers for Update() and Draw() calls
	__int64 CurrentTick, LastTick, TickFrequency;
	__int64 UpdateAccumulator = 0, DrawAccumulator = 0; //TODO: Evaluate if a tick-based accumulator is a problem.
	QueryPerformanceFrequency((LARGE_INTEGER*)& TickFrequency); //Gets the ticks per second.
	QueryPerformanceCounter((LARGE_INTEGER*)& CurrentTick); //Gets the system time in ticks
	LastTick = CurrentTick;
	__int64 UpdateDeltaInTicks = TickFrequency / UPDATE_SPEED;
	__int64 DrawDeltaInTicks = TickFrequency / DRAW_SPEED;

	// Main Loop:
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		
		// Dispatch Windows Messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// TODO: Abstract this code into Application:Tick() or Window:Tick().
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Time and trigger Core Calls
		else
		{
			app->Tick(); // CORE CALL!

			// Step timers
			LastTick = CurrentTick;
			QueryPerformanceCounter((LARGE_INTEGER*)& CurrentTick);
			UpdateAccumulator += CurrentTick - LastTick;
			DrawAccumulator += CurrentTick - LastTick;

			// Update
			if (UpdateAccumulator >= UpdateDeltaInTicks) {
				app->Update(); // CORE CALL!
				UpdateAccumulator -= UpdateDeltaInTicks;
			}
			// Draw
			if (DrawAccumulator >= DrawDeltaInTicks) {
				app->Draw();// app->Draw(double(UpdateAccumulator / UpdateDeltaInTicks)); // CORE CALL!
				DrawAccumulator = 0;
			}
		}
		//// TODO: Make DestroyWindow a platform-agnostic callback
		//if (!game.isRunning)
		//	DestroyWindow(hWnd);
	}

	delete app; // Clean up the Aplication
	CleanupDebugConsole();
	return (int)msg.wParam; // return this part of the WM_QUIT message to Windows
}

// WinProc:
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_ACTIVATEAPP:
		//game.isFocussed = wParam == TRUE;
		if (wParam == TRUE)
		{
			// TODO: Handle when the window gains focus
		}
		else
		{
			// TODO: Handle when the window loses focus
			//iInput.ClearInput();
		}
		return 0;
		break;
	case WM_CHAR:
		//TODO: Handle character entry (regular typing).
		return 0;
		break;
	case WM_MOUSEMOVE:
		// Inform Input of the cursor's current location:
		// iInput.UpdateMousePos(LOWORD(lParam), HIWORD(lParam));
		return 0;
		break;
	case WM_INPUT:
	{
		// Get size of header.
		UINT dwSize = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		// Use header to get raw input data.
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
			return 0; //TODO: Handle this raw input error.
		}
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* data = (RAWINPUT*)lpb; //cast data into useful format.

		// Send Raw Input data to be interpreted by the input mapper.
		//iInput.InterpretRawInputData(data);

		return 0;
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);		// Handle any messages the switch statement didn't
		break;
	}
	return 0;
}

// Window Helper Function
HWND CreateClientWindow(HINSTANCE hInstance)
{
	// Define window class
	HWND hWnd;												// Window handle
	WNDCLASSEX wc;											// Window Class info
	ZeroMemory(&wc, sizeof(WNDCLASSEX));					// Clear the WC for use

	wc.cbClsExtra = 0;										// Extra bits (unused).
	wc.cbWndExtra = 0;										// Extra bits (unused).
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;						// Note: DirectX overrides redraw styles.
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;							// Sets WindowProc() to receive Windows messages
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// TODO: Set up an icon per game title: look up MakeIntResource().
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);			// TODO: Set up with small icon per game title.
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// Default mouse cursor.
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);	// Fill color when window is redrawn (set to none).
	wc.lpszMenuName = NULL;									// Menu name (none, because we have no menus).
	wc.lpszClassName = "ENTERPRISEWNDCLASS";				// Friendly name for this window class.

	// Register window class
	if (!RegisterClassEx(&wc))
	{
		OutputDebugString("\nFAILED TO CREATE WINDOW CLASS\n");
		return NULL;
	}

	// Calculate initial window size.
	RECT wr = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
	DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Window style
	AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);

	// Create window.
	hWnd = CreateWindowEx(NULL,							// We're not using an extended window style
		"ENTERPRISEWNDCLASS",
		WIN_TITLE,										// Window title
		winStyle,
		800,											// X-position of the window
		150,											// Y-position of the window
		wr.right - wr.left,								// Width of the window
		wr.bottom - wr.top,								// Height of the window
		NULL,											// We have no parent window, NULL
		NULL,										    // We aren't using menus, NULL
		hInstance,
		NULL);											// We don't have multiple windows, NULL

	// Throw error message if window fails to create.
	if (!hWnd)
		OutputDebugString("\nFAILED TO CREATE WINDOW\n");

	return hWnd;
}

// Creates a debug console window.  Called in Init().
void InitializeDebugConsole()
{
	//Create a console for this application
	AllocConsole();
	//Redirect unbuffered STDOUT to the console
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
	FILE* COutputHandle = _fdopen(SystemOutput, "w");
	*stdout = *COutputHandle;
	std::setvbuf(stdout, NULL, _IONBF, 0);

	//Redirect unbuffered STDERR to the console
	HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
	int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
	FILE* CErrorHandle = _fdopen(SystemError, "w");
	*stderr = *CErrorHandle;
	std::setvbuf(stderr, NULL, _IONBF, 0);

	//Redirect unbuffered STDIN to the console
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
	FILE* CInputHandle = _fdopen(SystemInput, "r");
	*stdin = *CInputHandle;
	std::setvbuf(stdin, NULL, _IONBF, 0);

	//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
	std::ios::sync_with_stdio(true);
}
// Cleans up the console window.  Called in Cleanup().
void CleanupDebugConsole()
{
	//Write "Press any key to exit"
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD CharsWritten;
	WriteConsole(ConsoleOutput, "\nClient has closed.  Press any key to exit", 42, &CharsWritten, 0);
	//Disable line-based input mode so we can get a single character
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(ConsoleInput, 0);
	//Read a single character
	TCHAR InputBuffer;
	DWORD CharsRead;
	ReadConsole(ConsoleInput, &InputBuffer, 1, &CharsRead, 0);
}
#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Application/Application.h"


/// The application entry point in Win32 builds.
/// @param hInstance Handle to the application instance.
/// @param hPrevInstance (unused) Legacy item from 16-bit Windows.  Do not use.
/// @param lpCmdLine This application's command-line arguments (Unicode string).
/// @param nCmdShow Flag indicating Window's preference for how the window should display.
/// @return Application exit code.
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try {
		// TODO: Handle command line arguments

		// Create the Application
		Enterprise::Application app;

		// Main Loop:
		MSG msg = { 0 };
		do
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// Dispatch any Windows Messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} 
		while (app.Run());

		return EXIT_SUCCESS;
	}
	catch (Enterprise::Exceptions::AssertFailed& e) { exit(EXIT_FAILURE); }
}

#endif

#ifdef EP_PLATFORM_WINDOWS

#include "EP_PCH.h"
#include "Core.h"
#include "Enterprise/Application/Application.h"


// WinMain:
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
	catch (Enterprise::Exceptions::AssertFailed & e)
	{
		exit(EXIT_FAILURE);
	}
}

#endif

#ifdef _WIN32
#include "Runtime.h"
#include "Window/Window.h"
#include <Enterprise/Runtime.h>
#include <Enterprise/Core.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nCmdShow)
{
	try
	{
		// Populate command line options map
		HashName currentOption = HN("");
		for (int i = 0; i < __argc; i++)
		{
			if (__argv[i][0] == '-')
			{
				currentOption = HN(__argv[i]);
				Enterprise::Runtime::cmdLineOptions[currentOption];
			}
			else
			{
				Enterprise::Runtime::cmdLineOptions[currentOption].push_back(__argv[i]);
			}
		}

		// Create window (OGL context must exist for engine Init())
		Editor::Window::CreatePrimary();

		// Create runtimes
		Enterprise::Runtime::isEditor = true;
		Enterprise::Runtime engine;
		engine.isRunning = false; // Disable PIE at launch
		Editor::Runtime editor;

		// Enter main loop
		MSG msg = { 0 };
		do
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// Pump messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			engine.Run();
		} while (editor.Run());
	}
	catch (Enterprise::Exceptions::AssertFailed&) { exit(EXIT_FAILURE); }
	catch (Enterprise::Exceptions::FatalError&) { exit(EXIT_FAILURE); }

	Editor::Window::DestroyPrimary();

	return EXIT_SUCCESS;
}

#endif // _WIN32

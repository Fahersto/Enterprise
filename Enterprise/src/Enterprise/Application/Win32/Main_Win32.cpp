#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Application/Application.h"

using Enterprise::Application;

/// The application entry point in Win32 builds.
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try
	{
		// Store the command line arguments in Application
		HashName currentOption = HN("");
		for (int i = 0; i < __argc; i++)
		{
			if (__argv[i][0] == '-')
			{
				currentOption = HN(__argv[i]);
				Application::_cmdLineOptions[currentOption];
			}
			else
			{
				Application::_cmdLineOptions[currentOption].push_back(__argv[i]);
			}
		}

		// Create the Enterprise Application
		Application app;

		// TODO: Generate warnings for unused args

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
		} while (app.Run());
	}
	catch (Enterprise::Exceptions::AssertFailed&) { exit(EXIT_FAILURE); }
	catch (Enterprise::Exceptions::FatalError&) { exit(EXIT_FAILURE); }

	return EXIT_SUCCESS;
}

#endif

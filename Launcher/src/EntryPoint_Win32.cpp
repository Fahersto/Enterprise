#ifdef _WIN32
#include <Enterprise/Runtime.h>
#include <Enterprise/Core.h>

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

		// Enter main loop
		Enterprise::Runtime engine;
		MSG msg = { 0 };
		do
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// Pump messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} while (engine.Run());
	}
	catch (Enterprise::Exceptions::AssertFailed&) { exit(EXIT_FAILURE); }
	catch (Enterprise::Exceptions::FatalError&) { exit(EXIT_FAILURE); }

	return EXIT_SUCCESS;
}

#endif // _WIN32

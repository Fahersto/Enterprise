#include "EP_PCH.h"
#include "Core.h"

#include "Enterprise/Application/Application.h"

// Systems
#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Time/Time.h"

#include "Enterprise/Events/CoreEvents.h"

Enterprise::Application* app = nullptr;

// WinMain:
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try {
		// Create Console
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Init();
		#endif

		// TODO: Handle command line arguments here

		// Create the Application
		app = Enterprise::CreateApplication();

		// Main Loop:
		MSG msg = { 0 };
		do
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// Dispatch any Windows Messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} 
		while (app->Run());

		delete app; // Clean up the Aplication

		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Cleanup(); // Close the debug console
		#endif

		return EXIT_SUCCESS;
	}
	catch (Enterprise::Exceptions::AssertFailed & e)
	{
		// If AssertFailed is thrown, Exit in disgrace.
		delete app; //TODO: Evaluate potential memory leak if exception is thrown during application constructor.
		#ifdef EP_CONFIG_DEBUG
		Enterprise::Console::Cleanup(); // Close the debug console
		#endif
		exit(EXIT_FAILURE);
	}
}
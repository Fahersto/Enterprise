#include "EP_PCH.h"
#include "Core.h"

#include "Enterprise/Application/Application.h"
#include "Enterprise/Time/Time.h"

#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Events/CoreEvents.h"

#define SIMSPEED 50 //Number of SimSteps per second.
#define MAX_FRAMETIME 0.25 //Max length, in seconds, a frame should take.

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

		// Set up timers
		LARGE_INTEGER QPF, CurrentCount, PreviousCount, SimStep_QPC, MaxFrameTime_QPC, accumulator, FrameTime{};
		accumulator.QuadPart = 0;
		EP_ASSERT(QueryPerformanceFrequency(&QPF)); //Gets the frequency of the HPC.
		EP_ASSERT(QueryPerformanceCounter(&CurrentCount)); //Gets the current count of the HPC.
		PreviousCount = CurrentCount;
		SimStep_QPC.QuadPart = QPF.QuadPart * (1.0 / SIMSPEED); //number of counts a SimStep should take.
		MaxFrameTime_QPC.QuadPart = QPF.QuadPart * MAX_FRAMETIME; //maximum number of counts a frame should take.

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
		} while (app->Run());

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
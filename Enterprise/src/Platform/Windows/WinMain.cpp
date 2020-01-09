#include "EP_PCH.h"

#include "Enterprise/Application/Console.h"
#include "Enterprise/Application/Application.h"

#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Events/CoreEvents.h"

#define UPDATE_SPEED 60
#define DRAW_SPEED 60

// WinMain:
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// Create Console
	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Init();
	#endif

	// TODO: Handle command line arguments here
	
	// Create the Application
	auto app = Enterprise::CreateApplication();

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
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Dispatch any Windows Messages
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Time and trigger Core Calls

			// Tick
			app->Tick();

			LastTick = CurrentTick;
			QueryPerformanceCounter((LARGE_INTEGER*)& CurrentTick);
			UpdateAccumulator += CurrentTick - LastTick;
			DrawAccumulator += CurrentTick - LastTick;

			// Update
			if (UpdateAccumulator >= UpdateDeltaInTicks) {
				app->Update();
				UpdateAccumulator -= UpdateDeltaInTicks;
			}
			// Draw
			if (DrawAccumulator >= DrawDeltaInTicks) {
				app->Draw();// app->Draw(double(UpdateAccumulator / UpdateDeltaInTicks));
				DrawAccumulator = 0;
			}
		}
		//// TODO: Make DestroyWindow a platform-agnostic callback
		//if (!game.isRunning)
		//	DestroyWindow(hWnd);
	}

	delete app; // Clean up the Aplication

	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Cleanup(); // Close the debug console
	#endif
	
	return (int)msg.wParam; // return this part of the WM_QUIT message to Windows
}
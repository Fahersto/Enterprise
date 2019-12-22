#pragma once

/* WinMain.h
	This header defines the main function for Windows as well as the WinProc function.  This include
	is stripped out of EntryPoint.h when building for other platforms.
*/

// WinProc Function (Handles Windows messages)
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Window Helper Function
HWND CreateClientWindow(HINSTANCE hInstance);

void InitializeDebugConsole();
void CleanupDebugConsole();
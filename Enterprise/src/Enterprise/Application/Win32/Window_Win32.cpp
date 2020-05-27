#include "EP_PCH.h"
#include "Core.h"
#include "Window_Win32.h"

#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"
#include "Enterprise/Input/InputEvents.h"

#ifdef EP_PLATFORM_WINDOWS

namespace Enterprise
{
	// Windows-specific window create function ----------------------------------------------------
	#ifdef EP_PLATFORM_WINDOWS
	Window* Window::m_Instance = nullptr; //Must be instantiated here to avoid multiple definitions
	Window* Window::Create(const WindowSettings& settings) 
	{
		EP_ASSERT(!m_Instance); // Don't create multiple windows
		m_Instance = new Window_Win32(settings);
		return m_Instance;
	}
	#endif

	// Win32 message handler ----------------------------------------------------------------------
	LRESULT CALLBACK WinProc_Win32(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message)
		{
		// Clicked the close button
		case WM_CLOSE:
			Events::Dispatch(EventTypes::WindowClose);
			return 0;
			break;
		// Gained or lost focus
		case WM_ACTIVATEAPP:
			if (wParam == TRUE)
				Events::Dispatch(EventTypes::WindowFocus);
			else
				Events::Dispatch(EventTypes::WindowLostFocus);
			return 0;
			break;
		// Text entry
		case WM_CHAR:
			// TODO: Handle modifier keys
			Events::Dispatch(EventTypes::KeyChar, char(wParam));
			return 0;
			break;
		// Mouse cursor position change
		case WM_MOUSEMOVE:
			Events::Dispatch(EventTypes::MousePosition, std::pair<int, int>(LOWORD(lParam), HIWORD(lParam)));
			return 0;
			break;
		// Raw input (Mouse delta, keyboard state changes)
		case WM_INPUT:
		{
			UINT dwSize = 0; // Stores size of header
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)); //Get size of header

			// Get header
			LPBYTE lpb = new BYTE[dwSize];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)); //Get input data
			EP_ASSERT(lpb); // If lpb is NULL, there was a problem.
			RAWINPUT* data = (RAWINPUT*)lpb; //Cast input data
			// TODO: Use raw input data
			delete[] lpb;
			// TODO: investigate whether this would be faster on the stack.

			return 0;
			break;
		}
		// Everything else
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		return 0;
	}

	// Constructor --------------------------------------------------------------------------------
	Window_Win32::Window_Win32(const WindowSettings& settings) : Window(settings)
	{
		// Get handle to the application (Note, this might* pose problems for multithreading).
		HINSTANCE hInstance = GetModuleHandle(NULL);

		// Window Class Info
		WNDCLASSEX wc;											// Window Class info
		ZeroMemory(&wc, sizeof(WNDCLASSEX));					// Clear the WC for use
		wc.cbClsExtra = 0;										// Extra bits (unused).
		wc.cbWndExtra = 0;										// Extra bits (unused).
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;						// Note: DirectX overrides redraw styles.
		wc.hInstance = hInstance;
		wc.lpfnWndProc = WinProc_Win32;							// Sets WindowProc() to receive Windows messages
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// TODO: Set up an icon
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);			// TODO: Set up a small icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// Default mouse cursor.
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);	// Fill color when window is redrawn (set to none).
		wc.lpszMenuName = NULL;									// Menu name (none, because we have no menus).
		wc.lpszClassName = L"EP_WNDCLASS";						// Friendly name for this window class.
		
		// Register the class
		EP_ASSERT(RegisterClassEx(&wc)); // If zero, the window class failed to register for some reason.

		// Calculate initial window size.
		RECT wr = { 0, 0, m_Settings.Width, m_Settings.Height };
		DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Window style
		AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);

		// Create window.
		hWnd = CreateWindowEx(NULL,							// We're not using an extended window style
			L"EP_WNDCLASS",
			m_Settings.Title.c_str(),						// Window title
			winStyle,
			CW_USEDEFAULT,									// X-position of the window (TODO: Set this via command line?)
			CW_USEDEFAULT,									// Y-position of the window
			wr.right - wr.left,								// Width of the window
			wr.bottom - wr.top,								// Height of the window
			NULL,											// We have no parent window, NULL
			NULL,										    // We aren't using menus, NULL
			hInstance,
			NULL);											// We don't have multiple windows, NULL

		EP_ASSERT(hWnd); // If NULL, the window wasn't created for some reason.
		ShowWindow(hWnd, SW_SHOWNORMAL); // Tell Windows to display the window.
	}

	// Destructor ---------------------------------------------------------------------------------
	Window_Win32::~Window_Win32()
	{
		DestroyWindow(hWnd);
	}
}

#endif

#include "EP_PCH.h"
#include "Win32Window.h"

#include "Enterprise/Application/Console.h"

#include "Enterprise/Events/Dispatcher.h"
#include "Enterprise/Events/CoreEvents.h"


// Windows-specific window create function
#ifdef EP_PLATFORM_WINDOWS
namespace Enterprise {
	Window* Window::Create(const WindowSettings& settings) { return new Platform::Win32Window(settings); }
}
#endif

namespace Enterprise::Platform {

	// WinProc (Windows window event handler)
	LRESULT CALLBACK Win32WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message)
		{
		case WM_DESTROY:
			// TODO: Dispatch, Handle Are You Sure situations
			PostQuitMessage(0);
			return 0;
			break;
		case WM_ACTIVATEAPP:
			if (wParam == TRUE)
				EP_QUICKEVENT(Enterprise::Event::WindowFocus);
			else
				EP_QUICKEVENT(Enterprise::Event::WindowLostFocus);
			return 0;
			break;
		case WM_CHAR:
			// TODO: Handle modifier keys
			EP_QUICKEVENT(Enterprise::Event::KeyChar, char(wParam));
			return 0;
			break;
		case WM_MOUSEMOVE:
			EP_QUICKEVENT(Enterprise::Event::MousePosition, LOWORD(lParam), HIWORD(lParam));
			return 0;
			break;
		case WM_INPUT:
		{
			UINT dwSize = 0; // Stores size of header
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)); //Get size of header

			// Get header
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL) { return 0; } //TODO: Handle this raw input error
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)); //Get input data
			RAWINPUT* data = (RAWINPUT*)lpb; //Cast input data
			// TODO: Use raw input data
			delete[] lpb;
			// TODO: investigate whether this would be faster on the stack.

			return 0;
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam); // pass unhandled events to default winproc
			break;
		}
		return 0;
	}

	Win32Window::Win32Window(const WindowSettings& settings)
		: m_Settings(settings)
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
		wc.lpfnWndProc = Win32WinProc;							// Sets WindowProc() to receive Windows messages
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// TODO: Set up an icon
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);			// TODO: Set up a small icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// Default mouse cursor.
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);	// Fill color when window is redrawn (set to none).
		wc.lpszMenuName = NULL;									// Menu name (none, because we have no menus).
		wc.lpszClassName = L"ENTERPRISEWNDCLASS";				// Friendly name for this window class.
		
		if (!RegisterClassEx(&wc)) // Register the class
		{
			// If false, the window class failed to register for some reason.
			//EP_FATAL("A window class failed to register with Windows.  Window title: {}", m_Settings.Title);
			EP_APPLICATIONEVENT(Event::TerminateApplication, 1);
		}

		// Calculate initial window size.
		RECT wr = { 0, 0, m_Settings.Width, m_Settings.Height };
		DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Window style
		AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);

		// Create window.
		hWnd = CreateWindowEx(NULL,							// We're not using an extended window style
			L"ENTERPRISEWNDCLASS",
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

		if (!hWnd)
		{
			// If false, the window wasn't created for some reason.
			//EP_FATAL("An attempt to create a window failed in Windows.  Window title: {}", m_Settings.Title);
			EP_APPLICATIONEVENT(Event::TerminateApplication, 1);
		}
		else
			ShowWindow(hWnd, SW_SHOWNORMAL);					// Tell Windows to display the window. (TODO: Handle alternative start modes)
	}

	Win32Window::~Win32Window()
	{
		// Cleanup your shit!!!
	}
}
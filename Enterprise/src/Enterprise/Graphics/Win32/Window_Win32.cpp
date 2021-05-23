#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "../Window.h"
#include "Enterprise/Events/Events.h"

static HWND hWnd; // handle to Win32 window
static HDC hDC = nullptr; // device context
static HGLRC hRC = nullptr; // rendering context
static HPALETTE hPalette = nullptr; // custom palette (if needed)

/// The Win32 window procedure function for the game window.
/// @param hWnd The window handle.
/// @param message The message identifier.
/// @param wParam The word parameter value.
/// @param lParam The long parameter value.
/// @return The LRESULT response to the message.
LRESULT CALLBACK Win32_WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	using Enterprise::Events;

	switch (message)
	{
	case WM_CLOSE: // Clicked the close button
		Events::Dispatch(HN("WindowClose"));
		break;

	case WM_ACTIVATEAPP: // Gained or lost focus
		if (wParam == TRUE)
			Events::Dispatch(HN("WindowFocus"));
		else
			Events::Dispatch(HN("WindowLostFocus"));
		break;

	case WM_CHAR: // Text entry
		Events::Dispatch(HN("KeyChar"), char(wParam));
		break;

	case WM_MOUSEMOVE: // Mouse cursor position change
		Events::Dispatch(HN("MousePosition"), std::pair<int, int>(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_INPUT: // Raw Input API
	{
		UINT RIDataSize = sizeof(RAWINPUT);
		BYTE RIData[sizeof(RAWINPUT)];

		EP_VERIFY_SLOW(
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, RIData, &RIDataSize, sizeof(RAWINPUTHEADER))
			<= RIDataSize);

		Events::Dispatch(HN("Win32_RawInput"), (RAWINPUT*)RIData);

		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	case WM_SYSCOMMAND: // Prevent opening the window menu with the Alt key
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
			return 0;
		else
			return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


/// Sets up and displays the Win32 game window.
/// @param settings A struct reference containing the desired window configuration.
void Enterprise::Window::CreatePrimaryWindow()
{
	EP_ASSERT_NOREENTRY();

	// Get handle to the application (Note, this might* pose problems for multithreading).
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Window Class Info
	WNDCLASSEX wc;                                          // Window Class info
	ZeroMemory(&wc, sizeof(WNDCLASSEX));                    // Clear the WC for use
	wc.cbClsExtra = 0;                                      // Extra bits (unused).
	wc.cbWndExtra = 0;                                      // Extra bits (unused).
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;									// Needed for OpenGL
	wc.hInstance = hInstance;
	wc.lpfnWndProc = Win32_WinProc;                         // Sets WindowProc() to receive Windows messages
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);             // TODO: Set up an icon
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);           // TODO: Set up a small icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);               // Default mouse cursor.
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);  // Fill color when window is redrawn (set to none).
	wc.lpszMenuName = NULL;                                 // Menu name (none, because we have no menus).
	wc.lpszClassName = L"EP_WNDCLASS";                      // Friendly name for this window class.

	// Register the class
	EP_VERIFY(RegisterClassEx(&wc)); //TODO: Use EP_ASSERT_CODE here to call GetLastError().

	// Calculate initial window size.  TODO: Set up from INI
	RECT wr = { 0, 0, 500, 500 };
	DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;    // Window style
	AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);

	// Create window
	hWnd = CreateWindowEx(NULL,						// We're not using an extended window style
						  L"EP_WNDCLASS",
						  Constants::WindowTitle,	// Window title
						  winStyle,
						  CW_USEDEFAULT,			// X-position of the window
						  CW_USEDEFAULT,			// Y-position of the window
						  wr.right - wr.left,		// Width of the window
						  wr.bottom - wr.top,		// Height of the window
						  NULL,						// We have no parent window, NULL
						  NULL,						// We aren't using menus, NULL
						  hInstance,
						  NULL);					// We don't have multiple windows, NULL

	EP_ASSERT(hWnd); //TODO: Use EP_ASSERT_CODE here to use GetLastError().

	// Set up OpenGL
	hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.cAuxBuffers = 0;

	int pf = ChoosePixelFormat(hDC, &pfd);
	EP_ASSERT(pf != 0);
	EP_VERIFY_NEQ(SetPixelFormat(hDC, pf, &pfd), FALSE);
	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Creates a logical palette if needed by the specific system.
	if (pfd.dwFlags & PFD_NEED_PALETTE || pfd.iPixelType == PFD_TYPE_COLORINDEX)
	{
		int n = 1 << pfd.cColorBits;
		if (n > 256) { n = 256; }

		LOGPALETTE* lpPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +
												sizeof(PALETTEENTRY) * n);
		memset(lpPal, 0, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * n);
		lpPal->palVersion = 0x300;
		lpPal->palNumEntries = n;

		GetSystemPaletteEntries(hDC, 0, n, &lpPal->palPalEntry[0]);

		/* if the pixel type is RGBA, then we want to make an RGB ramp,
		otherwise (color index) set individual colors. */
		if (pfd.iPixelType == PFD_TYPE_RGBA)
		{
			int redMask = (1 << pfd.cRedBits) - 1;
			int greenMask = (1 << pfd.cGreenBits) - 1;
			int blueMask = (1 << pfd.cBlueBits) - 1;
			int i;

			/* fill in the entries with an RGB color ramp. */
			for (i = 0; i < n; ++i)
			{
				lpPal->palPalEntry[i].peRed =
					(((i >> pfd.cRedShift) & redMask) * 255) / redMask;
				lpPal->palPalEntry[i].peGreen =
					(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
				lpPal->palPalEntry[i].peBlue =
					(((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}
		else
		{
			lpPal->palPalEntry[0].peRed = 0;
			lpPal->palPalEntry[0].peGreen = 0;
			lpPal->palPalEntry[0].peBlue = 0;
			lpPal->palPalEntry[0].peFlags = PC_NOCOLLAPSE;
			lpPal->palPalEntry[1].peRed = 255;
			lpPal->palPalEntry[1].peGreen = 0;
			lpPal->palPalEntry[1].peBlue = 0;
			lpPal->palPalEntry[1].peFlags = PC_NOCOLLAPSE;
			lpPal->palPalEntry[2].peRed = 0;
			lpPal->palPalEntry[2].peGreen = 255;
			lpPal->palPalEntry[2].peBlue = 0;
			lpPal->palPalEntry[2].peFlags = PC_NOCOLLAPSE;
			lpPal->palPalEntry[3].peRed = 0;
			lpPal->palPalEntry[3].peGreen = 0;
			lpPal->palPalEntry[3].peBlue = 255;
			lpPal->palPalEntry[3].peFlags = PC_NOCOLLAPSE;
		}

		hPalette = CreatePalette(lpPal);
		if (hPalette)
		{
			SelectPalette(hDC, hPalette, FALSE);
			RealizePalette(hDC);
		}

		free(lpPal);
	}

	// Create context and init Glad
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	EP_VERIFY(gladLoadGL());

	// Display window
	ShowWindow(hWnd, SW_SHOWNORMAL);
}

void Enterprise::Window::DestroyPrimaryWindow()
{
	EP_ASSERT_NOREENTRY();
	EP_ASSERTF(hWnd,
			   "Window: Attempted to destroy primary window before creation.");

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd, hDC);
	wglDeleteContext(hRC);
	DestroyWindow(hWnd);
	if (hPalette)
	{
		DeleteObject(hPalette);
	}
}

void Enterprise::Window::SwapBuffers()
{
	glFlush();
	::SwapBuffers(hDC);
}

#endif

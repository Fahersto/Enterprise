#ifdef _WIN32
#include "Enterprise/Window.h"
#include "Enterprise/Core.h"
#include "Enterprise/Events.h"
#include "Enterprise/Runtime.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include "Enterprise/Core/Win32APIHelpers.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

using Enterprise::Window;
using Enterprise::Events;

static Window::WindowMode windowMode;
static int windowWidth;
static int windowHeight;
static float aspectRatio;

static HINSTANCE appHandle = NULL;
static HWND windowHandle = NULL;
static HDC deviceContext = NULL;
static HGLRC renderingContext = NULL;

static const PIXELFORMATDESCRIPTOR pixelFormatDescription =
{
	sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	32, // Color bits
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	24, 8, // Depth + stencil bits
	0, 0, 0, 0, 0, 0
};

LRESULT CALLBACK Win32_WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
		return TRUE;
		break;

	case WM_CLOSE:
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
		Events::Dispatch(HN("MousePosition"),
			glm::vec2(GET_X_LPARAM(lParam), float(windowHeight) - GET_Y_LPARAM(lParam)));
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

void Window::SwapBuffers()
{
	EP_ASSERT_SLOW(windowHandle);
	EP_ASSERT_SLOW(renderingContext);

	if (deviceContext == NULL)
	{
		deviceContext = ::GetDC(windowHandle);
		wglMakeCurrent(deviceContext, renderingContext);
	}
	::SwapBuffers(deviceContext);
}

void Window::SetMode(Window::WindowMode mode)
{
	// TODO: Implement
	windowMode = mode;
}

void Window::SetWidth(int width)
{
	// TODO: Implement
	windowWidth = width;
}

void Window::SetHeight(int height)
{
	// TODO: Implement
	windowHeight = height;
}

void Window::SetTitle(const std::string& title)
{
	// TODO: Implement
}

Window::WindowMode Window::GetMode()
{
	if (!windowHandle)
	{
		EP_ERROR("Window::GetMode(): Window does not exist!");
		return WindowMode::WindowedResizable;
	}

	return windowMode;
}

int Window::GetWidth()
{
	if (!windowHandle)
	{
		EP_ERROR("Window::GetWidth(): Window does not exist!");
		return 0;
	}

	return windowWidth;
}

int Window::GetHeight()
{
	if (!windowHandle)
	{
		EP_ERROR("Window::GetHeight(): Window does not exist!");
		return 0;
	}

	return windowHeight;
}

float Window::GetAspectRatio()
{
	if (!windowHandle)
	{
		EP_ERROR("Window::GetAspectRatio(): Window does not exist!");
		return 1.0f;
	}

	return aspectRatio;
}

void Window::Init()
{
	EP_ASSERT_NOREENTRY();

	Events::Subscribe(HN("WindowClose"), [](Events::Event &e){
						  Enterprise::Events::Dispatch(HN("QuitRequested"));
						  return true; });

	appHandle = ::GetModuleHandle(NULL);

	// Register window class
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Win32_WinProc;
	wc.hInstance = appHandle;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"EP_WNDCLASS";
#ifdef EP_BUILD_DYNAMIC
	wc.hIcon = ::LoadIcon(appHandle, L"ICON_ENTERPRISE");
#else
	wc.hIcon = ::LoadIcon(appHandle, L"ICON_GAME");
#endif // EP_BUILD_DYN
	EP_VERIFY(::RegisterClassEx(&wc));

	// TODO: Set the following details from INI, then from arguments, then from style
	std::string title = "Game Window Title";
	windowMode = WindowMode::WindowedResizable;
	windowWidth = 1280;
	windowHeight = 720;
	aspectRatio = (float)windowWidth / (float)windowHeight;
	DWORD winStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;
	DWORD exWinStyle = WS_EX_APPWINDOW;

	// Create window
	RECT wr = { 0, 0, (long)windowWidth, (long)windowHeight };
	::AdjustWindowRectEx(&wr, winStyle, FALSE, exWinStyle);
	int positionX = CW_USEDEFAULT, positionY = CW_USEDEFAULT;

	windowHandle = ::CreateWindowEx(exWinStyle,
									L"EP_WNDCLASS",
									UTF8toWCHAR(title).c_str(),
									winStyle,
									positionX, positionY,
									wr.right - wr.left, wr.bottom - wr.top,
									NULL, NULL,
									appHandle,
									NULL);
	EP_ASSERT(windowHandle);

	// Device context
	deviceContext = ::GetDC(windowHandle);
	int pf = ::ChoosePixelFormat(deviceContext, &pixelFormatDescription);
	EP_ASSERT(pf != 0);
	EP_VERIFY_NEQ(::SetPixelFormat(deviceContext, pf, &pixelFormatDescription), FALSE);

	// Rendering context
	renderingContext = wglCreateContext(deviceContext);
	EP_ASSERT(wglMakeCurrent(deviceContext, renderingContext));

	// GLAD
	EP_VERIFY(gladLoadGL());
	
	::ShowWindow(windowHandle, SW_SHOWNORMAL);
}

void Window::Cleanup()
{
	EP_ASSERT(windowHandle);
	EP_ASSERT(renderingContext);

	if (deviceContext)
	{
		wglMakeCurrent(NULL, NULL);
		::ReleaseDC(windowHandle, deviceContext);
		deviceContext = NULL;
	}

	wglDeleteContext(renderingContext);
	renderingContext = NULL;
	::DestroyWindow(windowHandle);
	windowHandle = NULL;
}

#endif // _WIN32

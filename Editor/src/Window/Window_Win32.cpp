#ifdef _WIN32
#include "Window.h"
#include "Runtime.h"
#include "Layouts/SceneEditor.h"
#include <Enterprise/Core.h>
#include <Enterprise/Events.h>
#include <Enterprise/File.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include "Enterprise/Core/Win32APIHelpers.h"

#include <glad/glad.h>
#include <Enterprise/Graphics/OpenGLHelpers.h>
#include <glm/glm.hpp>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"

using Enterprise::Events;
using Enterprise::File;

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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Win32_WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
	
	switch (message)
	{
	case WM_ERASEBKGND:
		return TRUE;
		break;

	case WM_SIZING:
		Editor::Runtime::Run();
		break;

	case WM_CLOSE:
		Events::Dispatch(HN("Editor_QuitRequested"));
		break;

	case WM_ACTIVATEAPP:
		if (wParam == TRUE)
			Events::Dispatch(HN("Editor_WindowFocus"));
		else
			Events::Dispatch(HN("Editor_WindowLostFocus"));
		break;

	case WM_CHAR:
		Events::Dispatch(HN("KeyChar"), char(wParam));
		break;

	case WM_MOUSEMOVE:
	{
		ImGuiIO io = ImGui::GetIO();
		if (io.WantCaptureMouse)
		{
			Events::Dispatch(HN("MousePosition"),
				glm::vec2(GET_X_LPARAM(lParam), io.DisplaySize.x - GET_Y_LPARAM(lParam)));
		}
		break;
	}
	case WM_INPUT: // Raw Input API
	{
		UINT RIDataSize = sizeof(RAWINPUT);
		BYTE RIData[sizeof(RAWINPUT)];

		EP_VERIFY_SLOW(
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, RIData, &RIDataSize, sizeof(RAWINPUTHEADER))
			<= RIDataSize);

		if (reinterpret_cast<RAWINPUT*>(RIData)->header.dwType == RIM_TYPEKEYBOARD)
		{
			ImGuiIO io = ImGui::GetIO();
			if(!io.WantCaptureKeyboard)
				Events::Dispatch(HN("Win32_RawInput"), (RAWINPUT*)RIData);
		}
		else if (reinterpret_cast<RAWINPUT*>(RIData)->header.dwType == RIM_TYPEMOUSE)
		{
			ImGuiIO io = ImGui::GetIO();
			if(!io.WantCaptureMouse)
				Events::Dispatch(HN("Win32_RawInput"), (RAWINPUT*)RIData);
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	// TODO: Review this
	// case WM_SYSCOMMAND: // Prevent opening the window menu with the Alt key
	// 	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
	// 		return 0;
	// 	else
	// 		return DefWindowProc(hWnd, message, wParam, lParam);
	// 	break;

	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

void Editor::Window::CreatePrimary()
{
	EP_ASSERT_NOREENTRY();

	appHandle = GetModuleHandle(NULL);
	ImGui_ImplWin32_EnableDpiAwareness();

	// Register window class
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = Win32_WinProc;
	wc.hInstance = appHandle;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"EP_EDITOR_WNDCLASS";
	wc.hIcon = ::LoadIcon(appHandle, L"ICON_ENTERPRISE");
	EP_VERIFY(::RegisterClassEx(&wc));

	// TODO: Set initial size and position from INI, then from defaults
	std::string title = "Enterprise Editor";
	int startingWidth = 1280, startingHeight = 720;
	int positionX = CW_USEDEFAULT, positionY = CW_USEDEFAULT;
	DWORD winStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;// | WS_MAXIMIZE (if starting maximized)

	// Create window
	RECT wr = { 0, 0, (long)startingWidth, (long)startingHeight};
	::AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);
	windowHandle = ::CreateWindowEx(WS_EX_APPWINDOW,
									L"EP_EDITOR_WNDCLASS",
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
	EP_VERIFY(wglMakeCurrent(deviceContext, renderingContext));

	// GLAD
	EP_VERIFY(gladLoadGL());

	// Window is not shown until Init() (after everything is initialized)
}

void Editor::Window::Init()
{
	// Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ConfigureImGui();
	ImGui_ImplWin32_InitForOpenGL(windowHandle, renderingContext);
	ImGui_ImplOpenGL3_Init("#version 410");

	::ShowWindow(windowHandle, SW_SHOWNORMAL);
}

void Editor::Window::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Editor::Window::EndFrame()
{
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	EP_GL(glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y));
	EP_GL(glClear(GL_COLOR_BUFFER_BIT));
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	if (deviceContext == NULL)
	{
		deviceContext = ::GetDC(windowHandle);
		wglMakeCurrent(deviceContext, renderingContext);
	}
	::SwapBuffers(deviceContext);
}

void Editor::Window::DestroyPrimary()
{
	EP_ASSERT(windowHandle);
	EP_ASSERT(renderingContext);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

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

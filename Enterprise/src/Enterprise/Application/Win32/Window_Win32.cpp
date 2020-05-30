#ifdef EP_PLATFORM_WINDOWS

#include "EP_PCH.h"
#include "Core.h"
#include "Window.h"

#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"
#include "Enterprise/Input/InputEvents.h"


// Win32 message handler ----------------------------------------------------------------------
LRESULT CALLBACK Win32_WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
        case WM_CLOSE: // Clicked the close button
            Events::Dispatch(EventTypes::WindowClose);
            break;
            
        case WM_ACTIVATEAPP: // Gained or lost focus
            if (wParam == TRUE)
                Events::Dispatch(EventTypes::WindowFocus);
            else
                Events::Dispatch(EventTypes::WindowLostFocus);
            break;
            
        case WM_CHAR: // Text entry
            // TODO: Handle modifier keys
            Events::Dispatch(EventTypes::KeyChar, char(wParam));
            break;
            
        case WM_MOUSEMOVE: // Mouse cursor position change
            Events::Dispatch(EventTypes::MousePosition, std::pair<int, int>(LOWORD(lParam), HIWORD(lParam)));
            break;
            
        case WM_INPUT: // Raw input API
            
            // Get size of header.  Needed for actually getting header.
            UINT dwSize = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            
            // Get header
            LPBYTE lpb = new BYTE[dwSize]; // TODO: Move this to the stack.
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
            EP_ASSERT(lpb); // If lpb is NULL, there was a problem.
            RAWINPUT* data = (RAWINPUT*)lpb; //Cast input data
            delete[] lpb;
            
            // TODO: Use raw input data
            break;
            
        default: // Pass unhandled messages back to the base procedure
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
    }
    return 0;
}
    

// Window class -----------------------------------------------------------------------------------

class Win32_Window : public Window
{
public:
    Win32_Window(const WindowSettings& settings) : Window(settings)
    {
        // Get handle to the application (Note, this might* pose problems for multithreading).
        HINSTANCE hInstance = GetModuleHandle(NULL);
        
        // Window Class Info
        WNDCLASSEX wc;                                            // Window Class info
        ZeroMemory(&wc, sizeof(WNDCLASSEX));                    // Clear the WC for use
        wc.cbClsExtra = 0;                                        // Extra bits (unused).
        wc.cbWndExtra = 0;                                        // Extra bits (unused).
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;                        // Note: DirectX overrides redraw styles.
        wc.hInstance = hInstance;
        wc.lpfnWndProc = Win32_WinProc;                            // Sets WindowProc() to receive Windows messages
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);                // TODO: Set up an icon
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);            // TODO: Set up a small icon
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);                // Default mouse cursor.
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);    // Fill color when window is redrawn (set to none).
        wc.lpszMenuName = NULL;                                    // Menu name (none, because we have no menus).
        wc.lpszClassName = L"EP_WNDCLASS";                        // Friendly name for this window class.
        
        // Register the class
        EP_ASSERT(RegisterClassEx(&wc)); // If zero, the window class failed to register for some reason.
        
        // Calculate initial window size.
        RECT wr = { 0, 0, m_Settings.Width, m_Settings.Height };
        DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;    // Window style
        AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);
        
        // Create window.
        hWnd = CreateWindowEx(NULL,                            // We're not using an extended window style
                              L"EP_WNDCLASS",
                              m_Settings.Title.c_str(),                        // Window title
                              winStyle,
                              CW_USEDEFAULT,                                    // X-position of the window
                              CW_USEDEFAULT,                                    // Y-position of the window
                              wr.right - wr.left,                                // Width of the window
                              wr.bottom - wr.top,                                // Height of the window
                              NULL,                                            // We have no parent window, NULL
                              NULL,                                            // We aren't using menus, NULL
                              hInstance,
                              NULL);                                            // We don't have multiple windows, NULL
        
        EP_ASSERT(hWnd); // If NULL, the window wasn't created for some reason.
        ShowWindow(hWnd, SW_SHOWNORMAL); // Tell Windows to display the window.
    }
    
    virtual ~Win32_Window() override
    {
        DestroyWindow(hWnd);
    }
    
private:
    HWND hWnd; // Win32 Window handle
};

// Windows-specific create function --------------------------------------------------------------
namespace Enterprise {
    Window* Window::m_Instance = nullptr;
    Window* Window::Create(const WindowSettings& settings)
    {
        EP_ASSERT(!m_Instance); // Don't create multiple windows
        m_Instance = new Win32_Window(settings);
        return m_Instance;
    }
}

#endif

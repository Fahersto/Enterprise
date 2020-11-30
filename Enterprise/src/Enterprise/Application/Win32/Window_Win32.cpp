#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Application/Window.h"

#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"
#include "Enterprise/Input/InputEvents.h"


/// The Win32 window procedure function.  Handles window events.
/// @param hWnd The window handle.
/// @param message The messaege identifier.
/// @param wParam The word parameter value.
/// @param lParam The long parameter value.
/// @return The LRESULT response to the message.
LRESULT CALLBACK Win32_WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    using Enterprise::Events;

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
            Events::Dispatch(EventTypes::KeyChar, char(wParam));
            break;
            
        case WM_MOUSEMOVE: // Mouse cursor position change
            Events::Dispatch(EventTypes::MousePosition, std::pair<int, int>(LOWORD(lParam), HIWORD(lParam)));
            break;

        case WM_INPUT: // Raw Input API
        {
            UINT RIDataSize = sizeof(RAWINPUT);
            BYTE RIData[sizeof(RAWINPUT)];

            EP_VERIFY_SLOW(
                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, RIData, &RIDataSize, sizeof(RAWINPUTHEADER))
                <= RIDataSize);

            Events::Dispatch(EventTypes::Win32_RawInput, (RAWINPUT*)RIData);

			return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
		default: // Pass unhandled messages back to the base procedure
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}
    

/// The Windows implementation of the game window.
/// @remarks This class derives from the generic Window class and represents the game window on the Windows platform.
///          Each platform has its own version of this.
class Win32_Window : public Enterprise::Window
{
public:

    /// Sets up and displays the Win32 game window.
    /// @param settings A struct reference containing the desired window configuration.
    Win32_Window(const WindowSettings& settings) : Window(settings)
    {
        // Get handle to the application (Note, this might* pose problems for multithreading).
        HINSTANCE hInstance = GetModuleHandle(NULL);
        
        // Window Class Info
        WNDCLASSEX wc;                                          // Window Class info
        ZeroMemory(&wc, sizeof(WNDCLASSEX));                    // Clear the WC for use
        wc.cbClsExtra = 0;                                      // Extra bits (unused).
        wc.cbWndExtra = 0;                                      // Extra bits (unused).
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;                     // Note: DirectX overrides redraw styles.
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

        // Calculate initial window size.
        RECT wr = { 0, 0, (LONG)m_Settings.Width, (LONG)m_Settings.Height };
        DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;    // Window style
        AdjustWindowRectEx(&wr, winStyle, FALSE, NULL);
        
        // Create window.
        hWnd = CreateWindowEx(NULL,                             // We're not using an extended window style
                              L"EP_WNDCLASS",
                              m_Settings.Title.c_str(),         // Window title
                              winStyle,
                              CW_USEDEFAULT,                    // X-position of the window
                              CW_USEDEFAULT,                    // Y-position of the window
                              wr.right - wr.left,               // Width of the window
                              wr.bottom - wr.top,               // Height of the window
                              NULL,                             // We have no parent window, NULL
                              NULL,                             // We aren't using menus, NULL
                              hInstance,
                              NULL);                            // We don't have multiple windows, NULL
        
        EP_ASSERT(hWnd); //TODO: Use EP_ASSERT_CODE here to use GetLastError().
        ShowWindow(hWnd, SW_SHOWNORMAL);
    }
    
    virtual ~Win32_Window() override
    {
        DestroyWindow(hWnd);
    }
    
private:
    HWND hWnd; // Handle to the Win32 window.
};


namespace Enterprise {
    Window* Window::m_Instance = nullptr;

    /// The Windows platform implementation of the window creation function.
    /// @param settings A struct reference containing the desired window configuration.
    /// @return Generic poointer to the game window object.
    /// @note @cm_Instance is deleted in the generic function @cWindow::Destroy()
    Window* Window::Create(const WindowSettings& settings)
    {
        EP_ASSERT(!m_Instance); // Don't create multiple windows
        m_Instance = new Win32_Window(settings);
        return m_Instance;
    }
}

#endif

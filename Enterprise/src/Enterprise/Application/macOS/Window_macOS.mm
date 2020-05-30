#ifdef EP_PLATFORM_MACOS

#include "EP_PCH.h"
#include "Core.h"
#include "Enterprise/Application/Window.h"

#include "Enterprise/Events/Events.h"
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"
//#include "Enterprise/Input/InputEvents.h"


// Window messages -------------------------------------------------------------------------------
@interface macOSWindowDelegate : NSWindow <NSWindowDelegate>
@end

@implementation macOSWindowDelegate

// The user has clicked the close button.
- (BOOL)windowShouldClose:(NSWindow*)sender
{
    Enterprise::Events::Dispatch(EventTypes::WindowClose);
    return NO; /// Window is closed automatically if the program terminates.  We don't do it here.
}

@end

// Window class  -----------------------------------------------------------------------------------
class macOS_Window : public Enterprise::Window
{
public:
    macOS_Window(const WindowSettings& settings) : Window(settings)
    {
        @autoreleasepool {
            
            // Set window style
            NSWindowStyleMask style = 0;
            style |= NSWindowStyleMaskClosable;
            style |= NSWindowStyleMaskTitled;
            style |= NSWindowStyleMaskFullSizeContentView;
            style |= NSWindowStyleMaskMiniaturizable;
            
            // Allocate window
            _windowReference = [[macOSWindowDelegate alloc] initWithContentRect:NSMakeRect(0, 0, 1080, 720)
                                                                      styleMask:style
                                                                        backing:NSBackingStoreBuffered
                                                                          defer:NO];
            
            // Creation success check
            EP_ASSERT( _windowReference != nil );
            
            // Set window properties
            NSString* convertedTitle = [[NSString alloc] initWithBytes:settings.Title.data()
                                                       length:settings.Title.size() * sizeof(wchar_t)
                                                     encoding:NSUTF32LittleEndianStringEncoding];
            
            [_windowReference setTitle: convertedTitle];
            [_windowReference setDelegate: _windowReference];
            [_windowReference setLevel:NSNormalWindowLevel];
            [_windowReference setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary
                                                 |NSWindowCollectionBehaviorDefault
                                                 |NSWindowCollectionBehaviorManaged
                                                 |NSWindowCollectionBehaviorParticipatesInCycle];
            [_windowReference setAcceptsMouseMovedEvents: YES];
            
            // Show and center the window
            [_windowReference makeKeyAndOrderFront:nil];
            [_windowReference center];
        }
    }
private:
    macOSWindowDelegate* _windowReference;
};

// macOS-specific window create function -----------------------------------------------------------
namespace Enterprise
{
    Window* Window::m_Instance = nullptr; // Must be defined here to avoid multiple inclusions
    Window* Window::Create(const WindowSettings& settings)
    {
        EP_ASSERT(!m_Instance); // Don't create multiple windows
        m_Instance = new macOS_Window(settings);
        return m_Instance;
    }
}

#endif

#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Core.h"
#include "Enterprise/Application/Window.h"

#include "Enterprise/Events/Events.h"
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"
#include "Enterprise/Input/InputEvents.h"


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

// The user has moved the window.
- (void)windowDidMove:(NSNotification *)notification
{
    // TODO: Set up consistent coordinate system across platforms
    Events::Dispatch(EventTypes::WindowMove, std::pair<int, int>(self.frame.origin.x, self.frame.origin.y));
}

// Window focus changed
- (void)windowDidBecomeKey:(NSNotification *)notification
{
    Events::Dispatch(EventTypes::WindowFocus);
}
- (void)windowDidResignKey:(NSNotification *)notification
{
    Events::Dispatch(EventTypes::WindowLostFocus);
}

// Keyboard input
- (void)keyDown:(NSEvent *)event
{
	Events::Dispatch(EventTypes::macOS_keyEvent, std::pair<unsigned short, bool>(event.keyCode, true));
}
- (void)keyUp:(NSEvent *)event
{
	Events::Dispatch(EventTypes::macOS_keyEvent, std::pair<unsigned short, bool>(event.keyCode, false));
}
- (void)flagsChanged:(NSEvent *)event
{
	Events::Dispatch(EventTypes::macOS_flagsChanged, (uint64_t)event.modifierFlags);
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
            _windowReference = [[macOSWindowDelegate alloc] initWithContentRect:NSMakeRect(0, 0,
                                                                                           settings.Width,
                                                                                           settings.Height)
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

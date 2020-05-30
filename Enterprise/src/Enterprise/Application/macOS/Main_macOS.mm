#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "Enterprise/Events/Events.h"
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"


/// Enterprise's macOS aapp delegate.  Used to handle messages from macOS.
@interface MacAppDelegate : NSObject <NSApplicationDelegate>
@end
@implementation MacAppDelegate

/// Invoked when the user selects "Quit" from the dock.
- (void)handleQuitFromDock:(NSAppleEventDescriptor*)Event withReplyEvent:(NSAppleEventDescriptor*)ReplyEvent {
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
}

/// Invoked when the user selects "Quit" from the menu bar.
- (void)handleQuitFromAppMenu {
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
}

// Init ================================================================================================================

/// Called right before the application opens.  Set up Cocoa stuff here.
- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    @autoreleasepool {
        
        // Set the custom callback for when Quit is selected from the Dock menu
        [[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
                                                           andSelector:@selector(handleQuitFromDock:withReplyEvent:)
                                                         forEventClass:kCoreEventClass
                                                            andEventID:kAEQuitApplication];
        
        // Set up the menu bar
        NSMenu* MenuBar = [NSMenu new];
        NSMenuItem* AppMenu = [NSMenuItem new]; // Application menu
        [MenuBar addItem:AppMenu];
        NSMenu* AppMenuImpl = [NSMenu new]; // Contents of application menu
        [AppMenu setSubmenu:AppMenuImpl];
        NSMenuItem* QuitOption = [[NSMenuItem alloc]initWithTitle:@"Quit" // Quit option
                                                           action:@selector(handleQuitFromAppMenu)
                                                    keyEquivalent:@""];
        [AppMenuImpl addItem:QuitOption];
        [NSApp setMainMenu:MenuBar]; // Register our choices
    }
}

// Entry Point / Main Loop =============================================================================================

- (void)applicationDidFinishLaunching:(NSNotification *)Notification {
    @autoreleasepool {
        try
        {
            // Create the app
            Enterprise::Application app;
            
            do
            {
                while( NSEvent *e = [NSApp nextEventMatchingMask: NSEventMaskAny
                                                       untilDate: nil
                                                          inMode: NSDefaultRunLoopMode
                                                         dequeue: true] ) {
                    [NSApp sendEvent: e]; // Dispatch OS messages
                }
            }
            while (app.Run()); // Run the app
        }
        catch (Enterprise::Exceptions::AssertFailed & e)
        {
            exit(EXIT_FAILURE);
        }
        
        /// Note: [terminate:] never returns.  App's destructor is called when leaving the try block.
        [NSApp terminate:self];
    }
}

@end

/// Create the native app and kick off the event loop.  We'll hijack the loop in applicationDidFinishLaunching:.
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setDelegate:[MacAppDelegate new]];
        [NSApp run];
    }
    return 0;
}

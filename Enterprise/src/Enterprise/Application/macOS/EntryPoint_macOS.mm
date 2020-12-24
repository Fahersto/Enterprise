#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/Events/Events.h"
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"

using Enterprise::Application;

/// Enterprise's macOS aapp delegate.  Used to handle messages from macOS.
@interface MacAppDelegate : NSObject <NSApplicationDelegate>
@end
@implementation MacAppDelegate

/// Invoked when the user selects "Quit" from the dock.
- (void)handleQuitFromDock:(NSAppleEventDescriptor*)Event withReplyEvent:(NSAppleEventDescriptor*)ReplyEvent
{
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
}

/// Invoked when the user selects "Quit" from the menu bar.
- (void)handleQuitFromAppMenu
{
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
}

// Set up Cocoa stuff
- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
    @autoreleasepool
	{
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

@end

/// The application entry point on macOS systems.
int main(int argc, const char * argv[])
{
    @autoreleasepool
	{
		// Create the NSApplication
        [NSApplication sharedApplication];

		// Set up custom app delegate
		MacAppDelegate * delegate = [[MacAppDelegate alloc] init];
		[NSApp setDelegate:delegate];

		// Activate and launch NSApp
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		[NSApp setPresentationOptions:NSApplicationPresentationDefault];
		[NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];

		try
		{
			// Populate command line options map
			HashName currentOption = HN("");
			for (int i = 0; i < argc; i++)
			{
				if (argv[i][0] == '-')
				{
					currentOption = HN(argv[i]);
					Application::_cmdLineOptions[currentOption];
				}
				else
				{
					Application::_cmdLineOptions[currentOption].push_back(argv[i]);
				}
			}
			
			// Enter main loop
			Application app;
			NSEvent *e;
			do
			{
				while((e = [NSApp nextEventMatchingMask: NSEventMaskAny
											  untilDate: nil
												 inMode: NSDefaultRunLoopMode
												dequeue: YES]))
				{
					// Pump messages
					[NSApp sendEvent: e];
					[NSApp updateWindows];
				}
			} while (app.Run());
		}
		catch (Enterprise::Exceptions::AssertFailed&){ exit(EXIT_FAILURE); }
		catch (Enterprise::Exceptions::FatalError&) { exit(EXIT_FAILURE); }
	}

    return EXIT_SUCCESS;
}

#endif // macOS

#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/Events/Events.h"
#include "Enterprise/Application/Application.h"
#include "Enterprise/Application/ApplicationEvents.h"


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

// Set up Cocoa stuff just before applictaion launch
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

/// Entry point to Enterprise app on macOS.
int main(int argc, const char * argv[])
{
    @autoreleasepool
	{
		// Create application
        [NSApplication sharedApplication];

		// Set up custom app delegate
		MacAppDelegate * delegate = [[MacAppDelegate alloc] init];
		[NSApp setDelegate:delegate];

		// Activate and launch the app
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		[NSApp setPresentationOptions:NSApplicationPresentationDefault];
		[NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];

		try
		{
			// Store the command line arguments in Application
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
			
			// Create the Application
			Enterprise::Application app;

			// TODO: Handle --help
			// TODO: Generate warnings for unused args

			NSEvent *e;
			do
			{
				do
				{
					e = [NSApp nextEventMatchingMask: NSEventMaskAny
										   untilDate: nil
											  inMode: NSDefaultRunLoopMode
											 dequeue: YES];
					if (e)
					{
						// Pump messages
						[NSApp sendEvent: e];
						[NSApp updateWindows];
					}
				} while (e);
			} while (app.Run()); // Loop condition steps the engine

			// app's destructor calls game cleanup code.
		}
		catch (Enterprise::Exceptions::AssertFailed&){ exit(EXIT_FAILURE); }
		catch (Enterprise::Exceptions::FatalError&) { exit(EXIT_FAILURE); }
    }
    return 0;
}

#endif

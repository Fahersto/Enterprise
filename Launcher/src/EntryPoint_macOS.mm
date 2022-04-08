#if defined(__APPLE__) && defined(__MACH__)

#include <Enterprise/Application.h>
#import <AppKit/AppKit.h>

using Enterprise::Application;

/// Enterprise's macOS app delegate.  Used to handle messages from macOS.
@interface MacAppDelegate : NSObject <NSApplicationDelegate>
@end
@implementation MacAppDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)Sender
{
	Enterprise::Events::Dispatch(HN("QuitRequested"));
	return NSTerminateCancel;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
    @autoreleasepool
	{
        // Set up the menu bar
        NSMenu* MenuBar = [[NSMenu new] autorelease];
        NSMenuItem* AppMenu = [[NSMenuItem new] autorelease]; // Application menu
        [MenuBar addItem:AppMenu];
        NSMenu* AppMenuImpl = [[NSMenu new] autorelease]; // Contents of application menu
        [AppMenu setSubmenu:AppMenuImpl];
        NSMenuItem* QuitOption = [[NSMenuItem alloc]initWithTitle:@"Quit" // Quit option
                                                           action:@selector(terminate:)
                                                    keyEquivalent:@""];
        [AppMenuImpl addItem:QuitOption];
        [NSApp setMainMenu:MenuBar];
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
		MacAppDelegate * delegate = [[[MacAppDelegate alloc] init] autorelease];
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
		catch (Enterprise::Exceptions::AssertFailed&) { exit(EXIT_FAILURE); }
		catch (Enterprise::Exceptions::FatalError&)   { exit(EXIT_FAILURE); }
	}

    return EXIT_SUCCESS;
}

#endif // macOS

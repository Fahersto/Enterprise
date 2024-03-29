#if defined(__APPLE__) && defined(__MACH__)
#include "Runtime.h"
#include "Window/Window.h"
#include <Enterprise/Runtime.h>
#import <AppKit/AppKit.h>

/// Enterprise's macOS app delegate.  Used to handle messages from macOS.
@interface MacAppDelegate : NSObject <NSApplicationDelegate>
@end
@implementation MacAppDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)Sender
{
	Enterprise::Events::Dispatch(HN("Editor_QuitRequested"));
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
					Enterprise::Runtime::cmdLineOptions[currentOption];
				}
				else
				{
					Enterprise::Runtime::cmdLineOptions[currentOption].push_back(argv[i]);
				}
			}

			// Create window (OGL context must exist for engine Init())
			Editor::Window::CreatePrimary();

			// Create runtimes
			Enterprise::Runtime::isEditor = true;
			Enterprise::Runtime engine;
			engine.isRunning = false; // Disable PIE at launch
			Editor::Runtime editor;

			// Enter main loop
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
				engine.Run();
			} while (editor.Run());
		}
		catch (Enterprise::Exceptions::AssertFailed&) { exit(EXIT_FAILURE); }
		catch (Enterprise::Exceptions::FatalError&)   { exit(EXIT_FAILURE); }
	}

	Editor::Window::DestroyPrimary();

	return EXIT_SUCCESS;
}

#endif // macOS

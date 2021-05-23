#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Core.h"
#include "../Window.h"
#include "Enterprise/Events/Events.h"

using Enterprise::Events;


@interface macOSWindowDelegate : NSWindow <NSWindowDelegate>
@end
@implementation macOSWindowDelegate

// The user has clicked the close button.
- (BOOL)windowShouldClose:(NSWindow*)sender
{
    Events::Dispatch(HN("WindowClose"));
    return NO; // Window is closed automatically if the program terminates.  We don't do it here.
}

// The user has moved the window.
- (void)windowDidMove:(NSNotification *)notification
{
    // TODO: Set up consistent coordinate system across platforms
    Events::Dispatch(HN("WindowMove"), std::pair<int, int>(self.frame.origin.x, self.frame.origin.y));
}

// Window focus changed
- (void)windowDidBecomeKey:(NSNotification *)notification
{
    Events::Dispatch(HN("WindowFocus"));
}
- (void)windowDidResignKey:(NSNotification *)notification
{
    Events::Dispatch(HN("WindowLostFocus"));
}

// Keyboard input
- (void)keyDown:(NSEvent *)event
{
	Events::Dispatch(HN("macOS_keyEvent"), std::pair<unsigned short, bool>(event.keyCode, true));
}
- (void)keyUp:(NSEvent *)event
{
	Events::Dispatch(HN("macOS_keyEvent"), std::pair<unsigned short, bool>(event.keyCode, false));
}
- (void)flagsChanged:(NSEvent *)event
{
	Events::Dispatch(HN("macOS_flagsChanged"), (uint64_t)event.modifierFlags);
}

@end


static macOSWindowDelegate* _windowReference;
static NSOpenGLView* view;


void Enterprise::Window::CreatePrimaryWindow()
{
	@autoreleasepool
	{
		NSRect viewingRect = NSMakeRect(0, 0, 500, 500); // TODO: Set from INI

		// Create OpenGL view
		NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
		{
			NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
			NSOpenGLPFAColorSize    , 24                           ,
			NSOpenGLPFAAlphaSize    , 8                            ,
			NSOpenGLPFADoubleBuffer ,
			NSOpenGLPFAAccelerated  ,
			0
		};
		NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
		view = [[NSOpenGLView alloc] initWithFrame:viewingRect pixelFormat:pixelFormat];

		// Make context current and set up Glad
		GLint swapInt = 1;
		[[view openGLContext] setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval];
		[[view openGLContext] makeCurrentContext];
		EP_VERIFY(gladLoadGL());

		// Allocate window
		NSWindowStyleMask style = NSWindowStyleMaskClosable
								| NSWindowStyleMaskTitled
								| NSWindowStyleMaskMiniaturizable;
		_windowReference = [[macOSWindowDelegate alloc] initWithContentRect:viewingRect
																  styleMask:style
																	backing:NSBackingStoreBuffered
																	  defer:NO];
		EP_ASSERT( _windowReference != nil );

		// Set window properties
		NSString* convertedTitle = [[[NSString alloc] initWithBytes:Constants::WindowTitle
															 length:wcslen(Constants::WindowTitle) * sizeof(wchar_t)
														   encoding:NSUTF32LittleEndianStringEncoding] autorelease];

		[_windowReference setTitle: convertedTitle];
		[_windowReference setDelegate: _windowReference];
		[_windowReference setLevel:NSNormalWindowLevel];
		[_windowReference setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary
												|NSWindowCollectionBehaviorDefault
												|NSWindowCollectionBehaviorManaged
												|NSWindowCollectionBehaviorParticipatesInCycle];
		[_windowReference setAcceptsMouseMovedEvents: YES];

		[_windowReference setOpaque:YES];
		[_windowReference setContentView:view];
		[_windowReference makeFirstResponder:view];

		// Show and center the window
		[_windowReference makeKeyAndOrderFront:nil];
		[_windowReference center];
	}
}

void Enterprise::Window::DestroyPrimaryWindow()
{
	EP_ASSERT_NOREENTRY();
	EP_ASSERTF(_windowReference, "Window: Attempted to destroy primary window before creation.");
}

void Enterprise::Window::SwapBuffers()
{
	glFlush();
	[[view openGLContext] flushBuffer];
}

#endif // macOS

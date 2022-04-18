#if defined(__APPLE__) && defined(__MACH__)
#import <AppKit/AppKit.h>
#include <glad/glad.h>
#include "Enterprise/Core.h"
#include "Enterprise/Window.h"
#include "Enterprise/Events.h"
#include "Enterprise/Graphics.h"

using Enterprise::Events;
using Enterprise::Window;

static Window::WindowMode windowMode = Window::WindowMode::WindowedFixed;
static unsigned int windowWidth;
static unsigned int windowHeight;
static float aspectRatio;

@interface macOSWindowDelegate : NSWindow <NSWindowDelegate>
@end
@implementation macOSWindowDelegate

- (BOOL)canBecomeKeyWindow { return YES; }
- (void)windowDidBecomeKey:(NSNotification *)notification { Events::Dispatch(HN("WindowFocus")); }
- (void)windowDidResignKey:(NSNotification *)notification { Events::Dispatch(HN("WindowLostFocus")); }

- (BOOL)windowShouldClose:(NSWindow*)sender { Events::Dispatch(HN("WindowClose")); return NO; }

- (void)windowWillEnterFullScreen:(NSNotification *)notification
{
	[self setFrame:[[NSScreen mainScreen] frame] display:YES];
	windowMode = Enterprise::Window::WindowMode::Fullscreen;
}
- (void)windowWillExitFullScreen:(NSNotification *)notification
{
	CGFloat xPos = NSWidth([[NSScreen mainScreen] frame]) / 2 - Window::GetWidth() / 2;
	CGFloat yPos = NSHeight([[NSScreen mainScreen] frame]) / 2 - Window::GetHeight() / 2;
	[self setFrame:NSMakeRect(xPos, yPos, Window::GetWidth(), Window::GetHeight()) display:YES];

	windowMode = Enterprise::Window::WindowMode::WindowedFixed;
}

- (void)windowDidMove:(NSNotification *)notification
{
    // TODO: Set up consistent coordinate system across platforms
    Events::Dispatch(HN("WindowMove"), std::pair<int, int>(self.frame.origin.x, self.frame.origin.y));
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

static macOSWindowDelegate* windowReference = nullptr;
static NSOpenGLView* view;

void Window::SwapBuffers()
{
	[[view openGLContext] flushBuffer];
}

void Window::SetMode(Window::WindowMode mode)
{
	@autoreleasepool
	{
		static NSRect windowedRect = [windowReference contentRectForFrameRect:[windowReference frame]];

		switch (mode)
		{
			case WindowMode::WindowedFixed:
			{
				if (windowMode == WindowMode::Fullscreen)
				{
					// Toggle fullscreen
					[windowReference toggleFullScreen:nil];
				}
				else if (windowMode == WindowMode::BorderlessFixed)
				{
					// Set resolution to windowed res
					[windowReference setFrame:windowedRect display:YES];

					// Set window style
					NSWindowStyleMask style = NSWindowStyleMaskClosable
											| NSWindowStyleMaskTitled
											| NSWindowStyleMaskMiniaturizable;
					[windowReference setStyleMask:style];
					[windowReference setHidesOnDeactivate:NO];
					[windowReference setLevel:NSNormalWindowLevel];
				}
				windowMode = WindowMode::WindowedFixed;
			}
				break;
			case WindowMode::BorderlessFixed:
			{
				if (windowMode == WindowMode::WindowedFixed)
				{
					windowedRect = [windowReference contentRectForFrameRect:[windowReference frame]];

					// Set window style
					[windowReference setStyleMask:NSWindowStyleMaskBorderless];
					[windowReference setHidesOnDeactivate:YES];
					[windowReference setLevel:NSMainMenuWindowLevel+1];

					// Set window size
					[windowReference setFrame:[[NSScreen mainScreen] frame] display:YES];
				}
				else if (windowMode == WindowMode::Fullscreen)
				{
					// Toggle fullscreen
					[windowReference toggleFullScreen:nil];

					// Set window style
					[windowReference setStyleMask:NSWindowStyleMaskBorderless];
					[windowReference setHidesOnDeactivate:YES];
					[windowReference setLevel:NSMainMenuWindowLevel+1];

					// Set window size
					[windowReference setFrame:[[NSScreen mainScreen] frame] display:YES];
				}
				windowMode = WindowMode::BorderlessFixed;
			}
				break;
			case WindowMode::Fullscreen:
			{
				// TODO: Remove title bar from fullscreen mode

				if (windowMode == WindowMode::WindowedFixed)
				{
					// Preserve windowed size
					windowedRect = [windowReference contentRectForFrameRect:[windowReference frame]];

					// Toggle fullscreen
					[windowReference toggleFullScreen:nil];
				}
				else if (windowMode == WindowMode::BorderlessFixed)
				{
					// Set style
					NSWindowStyleMask style = NSWindowStyleMaskClosable
											| NSWindowStyleMaskTitled
											| NSWindowStyleMaskMiniaturizable;
					[windowReference setStyleMask:style];
					[windowReference setHidesOnDeactivate:NO];
					[windowReference setLevel:NSNormalWindowLevel];

					// Toggle fullscreen
					[windowReference toggleFullScreen:nil];
				}
				// windowMode set automatically in window delegate
			}
				break;
			default:
				// TODO: Support resizeable modes
				EP_ASSERT_NOENTRY();
				break;
		}
	}
}

void Window::SetWidth(int width)
{
	// TODO: Implement
	windowWidth = width;
}

void Window::SetHeight(int height)
{
	// TODO: Implement
	windowHeight = height;
}

void Window::SetTitle(const std::string& title)
{
	// TODO: Implement
}

Window::WindowMode Window::GetMode()
{
	if (!windowReference)
	{
		EP_ERROR("Window::GetMode(): Window does not exist!");
		return WindowMode::WindowedResizable;
	}

	return windowMode;
}

int Window::GetWidth()
{
	if (!windowReference)
	{
		EP_ERROR("Window::GetWidth(): Window does not exist!");
		return 0;
	}

	return windowWidth;
}

int Window::GetHeight()
{
	if (!windowReference)
	{
		EP_ERROR("Window::GetHeight(): Window does not exist!");
		return 0;
	}

	return windowHeight;
}

float Window::GetAspectRatio()
{
	if (!windowReference)
	{
		EP_ERROR("Window::GetAspectRatio(): Window does not exist!");
		return 1.0f;
	}

	return aspectRatio;
}

void Window::Init()
{
	EP_ASSERT_NOREENTRY();

	// TODO: Set default size & position from INI, then from style.
	windowWidth = 1280;
	windowHeight = 720;
	aspectRatio = (double)windowWidth / (double)windowHeight;

	@autoreleasepool
	{
		CGFloat xPos = NSWidth([[NSScreen mainScreen] frame])  / 2 - windowWidth  / 2;
		CGFloat yPos = NSHeight([[NSScreen mainScreen] frame]) / 2 - windowHeight / 2;
		NSRect viewingRect = NSMakeRect(xPos, yPos, windowWidth, windowHeight);

		// Create OpenGL view
		NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
		{
			NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
			NSOpenGLPFAColorSize    , 24                           ,
			NSOpenGLPFAAlphaSize    , 8                            ,
			NSOpenGLPFADepthSize    , 24                           ,
			NSOpenGLPFAStencilSize  , 8                            ,
			NSOpenGLPFADoubleBuffer ,
			NSOpenGLPFAAccelerated  ,
			0
		};
		NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
		view = [[NSOpenGLView alloc] initWithFrame:viewingRect pixelFormat:pixelFormat];

		// Make context current and set up Glad
		GLint swapInt = 1; // TODO: Set from INI file
		[[view openGLContext] setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval];
		[[view openGLContext] makeCurrentContext];
		EP_VERIFY(gladLoadGL());

		// Allocate window
		NSWindowStyleMask style = NSWindowStyleMaskClosable
								| NSWindowStyleMaskTitled
								| NSWindowStyleMaskMiniaturizable;
		windowReference = [[macOSWindowDelegate alloc] initWithContentRect:viewingRect
																  styleMask:style
																	backing:NSBackingStoreBuffered
																	  defer:YES];

		EP_ASSERT( windowReference != nil );

		// Set window properties
		NSString* convertedTitle = [[[NSString alloc] initWithBytes:L"Enterprise Window Title"//Constants::WindowTitle
															 length:wcslen(L"Enterprise Window Title"/*Constants::WindowTitle*/) * sizeof(wchar_t)
														   encoding:NSUTF32LittleEndianStringEncoding] autorelease];

		[windowReference setTitle: convertedTitle];
		[windowReference setDelegate: windowReference];
		[windowReference setLevel:NSNormalWindowLevel];
		[windowReference setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary
											   | NSWindowCollectionBehaviorManaged];
		[windowReference setAcceptsMouseMovedEvents: YES];

		[windowReference setOpaque:YES];
		[windowReference setContentView:view];
		[windowReference makeFirstResponder:view];

		// Show window
		[windowReference makeKeyAndOrderFront:nil];
	}
}

void Window::Cleanup()
{
	EP_ASSERT_NOREENTRY();
	EP_ASSERTF(windowReference, "Window: Attempted to destroy primary window before creation.");
}

#endif // macOS

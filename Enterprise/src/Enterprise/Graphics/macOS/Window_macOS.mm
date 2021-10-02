#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Core.h"
#include "../Window.h"
#include "Enterprise/Events/Events.h"
#include "Enterprise/Graphics/Graphics.h"

using Enterprise::Events;
using Enterprise::Window;

static Window::WindowMode currentMode = Window::WindowMode::Windowed;

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
	currentMode = Enterprise::Window::WindowMode::Fullscreen;
}
- (void)windowWillExitFullScreen:(NSNotification *)notification
{
	CGFloat xPos = NSWidth([[NSScreen mainScreen] frame]) / 2 - Window::GetWidth() / 2;
	CGFloat yPos = NSHeight([[NSScreen mainScreen] frame]) / 2 - Window::GetHeight() / 2;
	[self setFrame:NSMakeRect(xPos, yPos, Window::GetWidth(), Window::GetHeight()) display:YES];

	currentMode = Enterprise::Window::WindowMode::Windowed;
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


unsigned int Window::windowWidth;
unsigned int Window::windowHeight;
float Window::aspectRatio;

static macOSWindowDelegate* _windowReference;
static NSOpenGLView* view;

void Window::CreatePrimaryWindow()
{
	EP_ASSERT_NOREENTRY();

	windowWidth = Constants::TEMP_WindowWidth;
	windowHeight = Constants::TEMP_WindowHeight;
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
		_windowReference = [[macOSWindowDelegate alloc] initWithContentRect:viewingRect
																  styleMask:style
																	backing:NSBackingStoreBuffered
																	  defer:YES];

		EP_ASSERT( _windowReference != nil );

		// Set window properties
		NSString* convertedTitle = [[[NSString alloc] initWithBytes:Constants::WindowTitle
															 length:wcslen(Constants::WindowTitle) * sizeof(wchar_t)
														   encoding:NSUTF32LittleEndianStringEncoding] autorelease];

		[_windowReference setTitle: convertedTitle];
		[_windowReference setDelegate: _windowReference];
		[_windowReference setLevel:NSNormalWindowLevel];
		[_windowReference setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary
											   | NSWindowCollectionBehaviorManaged];
		[_windowReference setAcceptsMouseMovedEvents: YES];

		[_windowReference setOpaque:YES];
		[_windowReference setContentView:view];
		[_windowReference makeFirstResponder:view];

		// Show window
		[_windowReference makeKeyAndOrderFront:nil];
	}
}

void Window::DestroyPrimaryWindow()
{
	EP_ASSERT_NOREENTRY();
	EP_ASSERTF(_windowReference, "Window: Attempted to destroy primary window before creation.");
}

void Window::SwapBuffers()
{
	[[view openGLContext] flushBuffer];
}


void Window::SetWindowMode(WindowMode mode)
{
	@autoreleasepool
	{
		static NSRect windowedRect = [_windowReference contentRectForFrameRect:[_windowReference frame]];

		switch (mode)
		{
			case WindowMode::Windowed:
			{
				if (currentMode == WindowMode::Fullscreen)
				{
					// Toggle fullscreen
					[_windowReference toggleFullScreen:nil];
				}
				else if (currentMode == WindowMode::Borderless)
				{
					// Set resolution to windowed res
					[_windowReference setFrame:windowedRect display:YES];

					// Set window style
					NSWindowStyleMask style = NSWindowStyleMaskClosable
											| NSWindowStyleMaskTitled
											| NSWindowStyleMaskMiniaturizable;
					[_windowReference setStyleMask:style];
					[_windowReference setHidesOnDeactivate:NO];
					[_windowReference setLevel:NSNormalWindowLevel];
				}
				currentMode = WindowMode::Windowed;
			}
				break;
			case WindowMode::Borderless:
			{
				if (currentMode == WindowMode::Windowed)
				{
					windowedRect = [_windowReference contentRectForFrameRect:[_windowReference frame]];

					// Set window style
					[_windowReference setStyleMask:NSWindowStyleMaskBorderless];
					[_windowReference setHidesOnDeactivate:YES];
					[_windowReference setLevel:NSMainMenuWindowLevel+1];

					// Set window size
					[_windowReference setFrame:[[NSScreen mainScreen] frame] display:YES];
				}
				else if (currentMode == WindowMode::Fullscreen)
				{
					// Toggle fullscreen
					[_windowReference toggleFullScreen:nil];

					// Set window style
					[_windowReference setStyleMask:NSWindowStyleMaskBorderless];
					[_windowReference setHidesOnDeactivate:YES];
					[_windowReference setLevel:NSMainMenuWindowLevel+1];

					// Set window size
					[_windowReference setFrame:[[NSScreen mainScreen] frame] display:YES];
				}
				currentMode = WindowMode::Borderless;
			}
				break;
			case WindowMode::Fullscreen:
			{
				// TODO: Remove title bar from fullscreen mode

				if (currentMode == WindowMode::Windowed)
				{
					// Preserve windowed size
					windowedRect = [_windowReference contentRectForFrameRect:[_windowReference frame]];

					// Toggle fullscreen
					[_windowReference toggleFullScreen:nil];
				}
				else if (currentMode == WindowMode::Borderless)
				{
					// Set style
					NSWindowStyleMask style = NSWindowStyleMaskClosable
											| NSWindowStyleMaskTitled
											| NSWindowStyleMaskMiniaturizable;
					[_windowReference setStyleMask:style];
					[_windowReference setHidesOnDeactivate:NO];
					[_windowReference setLevel:NSNormalWindowLevel];

					// Toggle fullscreen
					[_windowReference toggleFullScreen:nil];
				}
				// currentMode set automatically in window delegate
			}
				break;
			default:
				EP_ASSERT_NOENTRY();
				break;
		}
	}
}

#endif // macOS

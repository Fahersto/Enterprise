#include "Window.h"
#include "Runtime.h"
#include <Enterprise/Events.h>
#include <Enterprise/Window.h>

#import <AppKit/AppKit.h>
#include <glad/glad.h>
#include <Enterprise/Graphics/OpenGLHelpers.h>

#include "imgui.h"
#include "backends/imgui_impl_osx.h"
#include "backends/imgui_impl_opengl3.h"

using Enterprise::Events;
using Enterprise::Window;

static Window::WindowMode windowMode = Window::WindowMode::WindowedFixed;
static unsigned int windowWidth;
static unsigned int windowHeight;
static float aspectRatio;

static NSOpenGLView* view;

@interface editorWindowDelegate : NSWindow <NSWindowDelegate>
@end
@implementation editorWindowDelegate

- (BOOL)canBecomeKeyWindow { return YES; }
- (void)windowDidBecomeKey:(NSNotification *)notification { Events::Dispatch(HN("Editor_WindowFocus")); }
- (void)windowDidResignKey:(NSNotification *)notification { Events::Dispatch(HN("Editor_WindowLostFocus")); }

- (BOOL)windowShouldClose:(NSWindow*)sender { Events::Dispatch(HN("Editor_QuitRequested")); return NO; }

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
- (void)windowDidResize:(NSNotification *)notification
{
	Editor::Runtime::Run();
}

// Mouse input
// TODO: Implement mouse input for Enterprise
-(void)mouseDown:(NSEvent *)event           { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)rightMouseDown:(NSEvent *)event      { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)otherMouseDown:(NSEvent *)event      { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)mouseUp:(NSEvent *)event             { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)rightMouseUp:(NSEvent *)event        { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)otherMouseUp:(NSEvent *)event        { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)mouseMoved:(NSEvent *)event          { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)mouseDragged:(NSEvent *)event        { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)rightMouseMoved:(NSEvent *)event     { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)rightMouseDragged:(NSEvent *)event   { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)otherMouseMoved:(NSEvent *)event     { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)otherMouseDragged:(NSEvent *)event   { ImGui_ImplOSX_HandleEvent(event, view); }
-(void)scrollWheel:(NSEvent *)event         { ImGui_ImplOSX_HandleEvent(event, view); }

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

static editorWindowDelegate* windowReference = nullptr;

void Editor::Window::CreatePrimary()
{
	EP_ASSERT_NOREENTRY();

	// TODO: Set default size & position from INI, then from style.
	windowWidth = 1280;
	windowHeight = 720;
	aspectRatio = (float)windowWidth / (float)windowHeight;

	@autoreleasepool
	{
		CGFloat xPos = NSWidth([[NSScreen mainScreen] frame])  / 2 - windowWidth  / 2;
		CGFloat yPos = NSHeight([[NSScreen mainScreen] frame]) / 2 - windowHeight / 2;
		NSRect viewingRect = NSMakeRect(xPos, yPos, windowWidth, windowHeight);

		// Create OpenGL view
		NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
		{
			NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
			NSOpenGLPFAColorSize    , 32                           ,
			NSOpenGLPFADepthSize    , 24                           ,
			NSOpenGLPFAStencilSize  , 8                            ,
			NSOpenGLPFADoubleBuffer ,
			NSOpenGLPFAAccelerated  ,
			0
		};
		NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
		view = [[NSOpenGLView alloc] initWithFrame:viewingRect pixelFormat:pixelFormat];

		// Allocate window
		NSWindowStyleMask style = NSWindowStyleMaskClosable
								| NSWindowStyleMaskTitled
								| NSWindowStyleMaskMiniaturizable
								| NSWindowStyleMaskResizable;
		windowReference = [[editorWindowDelegate alloc] initWithContentRect:viewingRect
																  styleMask:style
																	backing:NSBackingStoreBuffered
																	  defer:YES];
		EP_ASSERT( windowReference != nil );

		// Set window properties
		NSString* convertedTitle = [[[NSString alloc] initWithBytes:L"Enterprise Editor"
															 length:wcslen(L"Enterprise Editor") * sizeof(wchar_t)
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

		// Make context current and set up Glad
		GLint swapInt = 1; // TODO: Set from INI file
		[[view openGLContext] setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval];
		[[view openGLContext] makeCurrentContext];
		EP_VERIFY(gladLoadGL());
	}
}

void Editor::Window::Init()
{
	// Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ConfigureImGui();
	ImGui_ImplOSX_Init(view);
	ImGui_ImplOpenGL3_Init("#version 410");

	[windowReference makeKeyAndOrderFront:nil];
}

void Editor::Window::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplOSX_NewFrame(view);
	ImGui::NewFrame();
}

void Editor::Window::EndFrame()
{
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	[[view openGLContext] makeCurrentContext];
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// EP_GL(glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y));
    GLsizei width  = (GLsizei)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    GLsizei height = (GLsizei)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	EP_GL(glViewport(0, 0, width, height));
	EP_GL(glClear(GL_COLOR_BUFFER_BIT));
	ImGui_ImplOpenGL3_RenderDrawData(draw_data);

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	[[view openGLContext] flushBuffer];
}

void Editor::Window::DestroyPrimary()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplOSX_Shutdown();
	ImGui::DestroyContext();
}

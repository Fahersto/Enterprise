#include "EP_PCH.h"

#include "Graphics.h"
#include "Window.h"

#include "WindowEvents.h"
#include "Enterprise/Application/ApplicationEvents.h"

using Enterprise::Graphics;


static bool OnWindowClose(Events::Event& e)
{
	EP_ASSERT(e.Type() == EventTypes::WindowClose);

    // By default, closing the window is equivalent to quitting from the OS.
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
    return true;
}

void Graphics::Init()
{
    Events::SubscribeToType(EventTypes::WindowClose, OnWindowClose);
    Window::CreatePrimaryWindow();

	// Set up OpenGL global vertex array
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void Graphics::Update()
{
	ClearRenderTarget();

	// Drawing code here

    Window::SwapBuffers();
}

void Graphics::Cleanup()
{
    Window::DestroyPrimaryWindow();
}

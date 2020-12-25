#include "EP_PCH.h"

#include "Graphics.h"
#include "Window.h"

#include "WindowEvents.h"
#include "Enterprise/Application/ApplicationEvents.h"

static bool OnWindowClose(Events::Event& e)
{
    EP_ASSERT(e.Type() == EventTypes::WindowClose);

    // By default, closing the window is equivalent to quitting from the OS.
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
    return true;
}

void Enterprise::Graphics::Init()
{
    Events::SubscribeToType(EventTypes::WindowClose, OnWindowClose);
    Window::CreatePrimaryWindow();
}

void Enterprise::Graphics::Update()
{
    /* rotate a triangle around */
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRotatef(1.0f, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);
	glIndexi(1);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2i(0, 1);
	glIndexi(2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2i(-1, -1);
	glIndexi(3);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2i(1, -1);
	glEnd();
	
    Window::SwapBuffers();
}

void Enterprise::Graphics::Cleanup()
{
    Window::DestroyPrimaryWindow();
}

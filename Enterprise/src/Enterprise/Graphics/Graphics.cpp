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
	static float intensity = 0.0f;
	intensity += 0.01f;
	if (intensity >= 1.0f) intensity -= 1.0f;

    // Simple clear color test
	glClearColor(intensity, intensity, intensity, 0);
	glClear(GL_COLOR_BUFFER_BIT);

    Window::SwapBuffers();
}

void Enterprise::Graphics::Cleanup()
{
    Window::DestroyPrimaryWindow();
}

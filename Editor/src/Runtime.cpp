#include "Runtime.h"
#include "Window/Window.h"
#include "Layouts/SceneEditor.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"

#include <Enterprise/Events.h>
using Enterprise::Events;

namespace Editor
{

bool Runtime::isRunning = true;

bool Runtime::Run()
{
	Window::StartFrame();
	SceneEditor::Draw();
	Window::EndFrame();
	return isRunning;
}

Runtime::Runtime()
{
	Events::Subscribe(HN("Editor_QuitRequested"), [](Events::Event &e)
					  {
						  Quit();
						  return true;
					  });

	SceneEditor::Init();
	Window::Init();
}

Runtime::~Runtime()
{
	SceneEditor::Cleanup();
}

}

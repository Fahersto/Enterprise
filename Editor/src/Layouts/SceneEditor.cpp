#include "Layouts/SceneEditor.h"
#include "Views/Scene.h"
#include "Views/Hierarchy.h"
#include "Views/Inspector.h"
#include "Views/Content.h"
#include "Runtime.h"

#include <Enterprise/File.h>
#include <Enterprise/Graphics.h>
#include "imgui.h"

using Enterprise::File;
using Enterprise::Graphics;

namespace Editor::SceneEditor
{
	void Init()
	{
		Views::Scene::Init();
		Views::Hierarchy::Init();
		Views::Inspector::Init();
		Views::Content::Init();
	}

	void Draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Scene", "Ctrl+N"))
					{
					}
					if (ImGui::MenuItem("Project"))
					{
					}
					ImGui::EndMenu();
				}
				if(ImGui::MenuItem("Open...", "Ctrl+O"))
				{
				}
				if (ImGui::BeginMenu("Open Recent"))
				{
					ImGui::MenuItem("ExampleProject.epproj");
					ImGui::MenuItem("ExampleScene.epscene");
					ImGui::MenuItem("ExampleScene2.epscene");
					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
				}
				if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
				{
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close Scene", "Ctrl+Shift+W"))
				{
				}
				if (ImGui::MenuItem("Quit", "Ctrl+Q"))
				{
					Editor::Runtime::Quit();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
				if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}

				ImGui::Separator();

				if (ImGui::MenuItem("Cut", "Ctrl+X", false, false)) {}
				if (ImGui::MenuItem("Copy", "Ctrl+C", false, false)) {}
				if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {}
				if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, false)) {}
				if (ImGui::MenuItem("Delete", "Del", false, false)) {}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Scene"))
					ImGui::SetWindowFocus("Scene");
				if (ImGui::MenuItem("Hierarchy"))
					ImGui::SetWindowFocus("Hierarchy");
				if (ImGui::MenuItem("Inspector"))
					ImGui::SetWindowFocus("Inspector");
				if (ImGui::MenuItem("Content"))
					ImGui::SetWindowFocus("Content");

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Documentation", "F1")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// Dockspace
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("Main Dockspace");
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Main Dockspace", nullptr,
					 ImGuiWindowFlags_NoDocking |
						 ImGuiWindowFlags_NoTitleBar |
						 ImGuiWindowFlags_NoCollapse |
						 ImGuiWindowFlags_NoResize |
						 ImGuiWindowFlags_NoMove |
						 ImGuiWindowFlags_NoBringToFrontOnFocus |
						 ImGuiWindowFlags_NoNavFocus);
		ImGui::PopStyleVar(3);
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();

		// Views
		Views::Scene::Draw();
		Views::Hierarchy::Draw();
		Views::Inspector::Draw();
		Views::Content::Draw();

		// TODO: Remove this
		ImGui::ShowDemoWindow();
	}

	void Cleanup()
	{
		Views::Scene::Cleanup();
		Views::Hierarchy::Cleanup();
		Views::Inspector::Cleanup();
		Views::Content::Cleanup();
	}
}

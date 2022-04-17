#include "Views/Hierarchy.h"
#include "imgui.h"

void Editor::Views::Hierarchy::Init()
{
}

void Editor::Views::Hierarchy::Draw()
{
	if (ImGui::Begin("Hierarchy"))
	{
		if (ImGui::TreeNode("Entity 1"))
		{
			ImGui::Selectable("Component 1");
			ImGui::Selectable("Component 2");
			ImGui::Selectable("Component 3");
			ImGui::Selectable("Component 4");
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Entity 2"))
		{
			ImGui::Selectable("Component 1");
			ImGui::Selectable("Component 2");
			ImGui::Selectable("Component 3");
			ImGui::Selectable("Component 4");
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void Editor::Views::Hierarchy::Cleanup()
{
}

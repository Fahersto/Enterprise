#include "Views/Scene.h"
#include "imgui.h"
#include <Enterprise/Graphics.h>

using Enterprise::Graphics;

static Graphics::FramebufferHandle sceneFB;

void Editor::Views::Scene::Init()
{
	Graphics::FramebufferAttachmentSpec textureAttachment;
	textureAttachment.format = ImageFormat::RGB8;
	textureAttachment.magFilter = TextureFilter::Nearest;
	textureAttachment.minFilter = TextureFilter::Nearest;
	textureAttachment.makeTexture = true;
	sceneFB = Graphics::CreateFramebuffer({textureAttachment}, 1, 1);
}

void Editor::Views::Scene::Draw()
{
	// Scene view
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if(ImGui::Begin("Scene"))
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;
		if(ImGui::IsWindowDocked())
		{
			vMin.x += ImGui::GetStyle().ChildBorderSize;
			vMin.y += std::max(ImGui::GetStyle().ChildBorderSize, ImGui::GetStyle().FrameBorderSize);
			vMax.x -= ImGui::GetStyle().ChildBorderSize;
			vMax.y -= ImGui::GetStyle().ChildBorderSize;
		}
		else
		{
			vMin.x += ImGui::GetStyle().WindowBorderSize;
			vMin.y += ImGui::GetStyle().WindowBorderSize;
			vMax.x -= ImGui::GetStyle().WindowBorderSize;
			vMax.y -= ImGui::GetStyle().WindowBorderSize;
		}

		Graphics::ResizeFramebuffer(sceneFB, vMax.x - vMin.x, vMax.y - vMin.y);
		Graphics::PushFramebuffer(sceneFB);
		Graphics::SetViewport(0, 1, 0, 1);

		Graphics::ClearCurrentViewport();
		// TODO: Render scene view here

		Graphics::PopFramebuffer();
		Graphics::TextureHandle viewportTex = Graphics::GetColorTexture(sceneFB, 0);
		ImGui::GetWindowDrawList()->AddImage(reinterpret_cast<ImTextureID>(viewportTex), ImVec2(vMin.x, vMax.y), ImVec2(vMax.x, vMin.y));
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void Editor::Views::Scene::Cleanup()
{
	Graphics::DeleteFramebuffer(sceneFB);
}

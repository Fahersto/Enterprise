#pragma once

namespace Editor::Window
{
	void CreatePrimary();

	void Init();
	void ConfigureImGui();
	
	void StartFrame();
	void EndFrame();
	
	void DestroyPrimary();
}

#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

namespace Constants
{
extern const unsigned int TEMP_WindowWidth;
extern const unsigned int TEMP_WindowHeight;

extern const wchar_t* WindowTitle;
}

class Window
{
public:
	static void CreatePrimaryWindow();
	static void DestroyPrimaryWindow();
	static void SwapBuffers();

	enum class WindowMode
	{
		Windowed,
		Borderless,
		Fullscreen
	};
	static void SetWindowMode(WindowMode mode);

	static unsigned int GetWidth() { return windowWidth; }
	static unsigned int GetHeight() { return windowHeight; }
	static float GetAspectRatio() { return aspectRatio; }

private:
	static unsigned int windowWidth;
	static unsigned int windowHeight;
	static float aspectRatio;

};

}

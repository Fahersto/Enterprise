#pragma once
#include "Enterprise/Core.h"

namespace Enterprise
{

namespace Constants
{/*
extern const unsigned int TEMP_WindowWidth;
extern const unsigned int TEMP_WindowHeight;*/

//extern const wchar_t* WindowTitle;
}

class Window
{
public:
	EP_API static void CreatePrimaryWindow();
	EP_API static void DestroyPrimaryWindow();
	EP_API static void SwapBuffers();

	enum class WindowMode
	{
		Windowed,
		Borderless,
		Fullscreen
	};
	EP_API static void SetWindowMode(WindowMode mode);

	EP_API static unsigned int GetWidth() { return windowWidth; }
	EP_API static unsigned int GetHeight() { return windowHeight; }
	EP_API static float GetAspectRatio() { return aspectRatio; }

private:
	static unsigned int windowWidth;
	static unsigned int windowHeight;
	static float aspectRatio;

};

}

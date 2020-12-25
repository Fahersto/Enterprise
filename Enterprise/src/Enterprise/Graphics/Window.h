#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

namespace Constants
{
/// The title of the game window.
extern const wchar_t* WindowTitle;
}

class Window
{
public:
	/// Creates the main application window.
	static void CreatePrimaryWindow();

	/// Destroys the main application window.
	static void DestroyPrimaryWindow();

	/// Swaps the graphics buffer and displays it.
	static void SwapBuffers();

};

}

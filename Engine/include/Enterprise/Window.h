#pragma once
#include "Enterprise/Core.h"

// TODO: Delete this after getting rid of SPDLog
#ifdef CreateWindow
#undef CreateWindow
#endif
#ifdef DestroyWindow
#undef DestroyWindow
#endif

namespace Enterprise
{

class EP_API Window
{
public:
	enum class WindowMode
	{
		WindowedFixed,
		WindowedResizable,
		BorderlessFixed,
		BorderlessResizable,
		Fullscreen
	};

	static void SwapBuffers();

	static void SetMode(WindowMode mode);
	static void SetWidth(int width);
	static void SetHeight(int height);
	static void SetTitle(const std::string& title);

	static WindowMode GetMode();
	static int GetWidth();
	static int GetHeight();
	static float GetAspectRatio();

private:
	friend class Runtime;

	static void Init();
	static void Cleanup();
};

}

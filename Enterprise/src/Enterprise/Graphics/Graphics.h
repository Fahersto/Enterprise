#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

class Graphics
{
public:

private:
	friend class Application;

	static void Init();
	static void Update();
	static void Cleanup();
};

}

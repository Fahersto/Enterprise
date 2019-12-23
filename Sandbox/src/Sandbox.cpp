#include "EP_PCH.h"
#include <Enterprise.h>

class Sandbox : public Enterprise::Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	Sandbox()
	{
		//TODO: Conditionalize this between debug and release builds.
		EP_DEBUG("Sandbox instantiated!");
	}

	//Called as rapidly as possible
	virtual void Tick() override 
	{

	}

	//Called at update rate (consistent timestep)
	virtual void Update() override
	{

	}

	//Called when rendering a new frame (variable timestep)
	virtual void Draw() override
	{

	}

	//Called at program end
	~Sandbox()
	{
		EP_DEBUG("Sandbox destroyed.");
	}
};

//EP_STARTSTATE(Sandbox);

Enterprise::Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new Sandbox();
}
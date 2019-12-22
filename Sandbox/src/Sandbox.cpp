#include "EP_PCH.h"
#include <Enterprise.h>

class Sandbox : public Enterprise::Application
{
public:
	//Called before everything else in the application.  Create windows and set up initial game state here.
	Sandbox()
	{
		#ifdef EP_DEBUG
		// Set Initial conditions for testing
		// TODO: set this with a JSON or LUA file?
		#else
		// Set Initial conditions for final game
		#endif

		Enterprise::Console::Init();
		EP_TRACE("Trace from Client");
		EP_WARN("Warn from Client");
		EP_INFO("Info from Client");
		EP_ERROR("Error from Client");
		EP_FATAL("Fatal from Client");
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

	//Called 
	~Sandbox()
	{
	}

};

//EP_STARTSTATE(Sandbox);

Enterprise::Application* Enterprise::CreateApplication()
{
	// Add any game-specific pre-launch code here.
	return new Sandbox();
}
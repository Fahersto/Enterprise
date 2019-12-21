#include <Enterprise.h>
#include <stdio.h>

class Sandbox : public Enterprise::Application
{
public:
	Sandbox()
	{
	}

	void Init() override
	{
	}

	void Tick() override {

	}

	void Update() override
	{
	}

	void Draw() override
	{
	}

	void Cleanup() override {
	}

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
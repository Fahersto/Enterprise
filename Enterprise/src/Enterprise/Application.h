#include "Console.h"

namespace Enterprise
{
	class Application
	{
	private:
		//Enterprise::Console* consoleImpl;
	public:
		Application();
		virtual void Init() = 0;
		virtual void Tick() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;
		virtual void Cleanup() = 0;
		virtual ~Application();
	};

	//To be defined in Client
	Application* CreateApplication();
}
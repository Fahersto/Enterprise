namespace Enterprise
{
	class Application
	{
	public:
		Application(); // Called at program start
		virtual void Tick() = 0; // Called as frequently as possible
		virtual void Update() = 0; // Called at Update Rate (fixed timestep)
		virtual void Draw() = 0; // Called when rendering (based on framerate settings)
		virtual ~Application(); // Called at program end
	};

	// Creator/getter for client application.  Defined in Client.
	Application* CreateApplication();
}
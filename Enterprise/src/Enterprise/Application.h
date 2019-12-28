namespace Enterprise
{
	class Application
	{
	public:
		Application(); // Called at program start
		void Tick(); // Called as frequently as possible
		void Update(); // Called at Update Rate (fixed timestep)
		void Draw(); // Called when rendering (based on framerate settings)
		virtual ~Application(); // Called at program end
	};

	// Creator/getter for client application.  Defined in Client.
	Application* CreateApplication();
}
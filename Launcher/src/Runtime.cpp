#include "Runtime.h"
#include <Enterprise/Runtime.h>
#include <Enterprise/GameEntryPoint.h>
#include <Enterprise/Core/Win32APIHelpers.h>

static Enterprise::Runtime* engine = nullptr;

namespace Launcher
{

Runtime::Runtime()
{
	engine = new Enterprise::Runtime;

	// Load game module
	if (Enterprise::Runtime::CheckCmdLineOption(HN("--project")))
	{
		std::vector<std::string> projectOptionArgs = Enterprise::Runtime::GetCmdLineOption(HN("--project"));
		if (projectOptionArgs.size())
		{
			if (Enterprise::LoadGameModule(projectOptionArgs.front()))
			{
				GameSysInit();
				GameInit();
			}
			else
			{
				std::string errorMsg = "Game module load failure!  Project file: " + projectOptionArgs.front();
				Enterprise::Platform::DisplayErrorDialog(errorMsg);
				engine->Quit();
			}
		}
		else
		{
			Enterprise::Platform::DisplayErrorDialog("Project was not specified!  "
													 "Specify a project file with \"-p\" or \"--project\"");
			engine->Quit();
		}
	}
	else
	{
		Enterprise::Platform::DisplayErrorDialog("Project was not specified!  "
												 "Specify a project file with \"-p\" or \"--project\"");
		engine->Quit();
	}
}

bool Runtime::Run()
{
	return engine->Run();
}

Runtime::~Runtime()
{
	GameCleanup();
	GameSysCleanup();

	if (engine)
		delete engine;

	Enterprise::UnloadGameModule();
}

}

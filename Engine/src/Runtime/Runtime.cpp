#include "Enterprise/Runtime.h"
#include "Enterprise/Core.h"

// Systems
#include "Enterprise/Window.h"
#include "Enterprise/Time.h"
#include "Enterprise/File.h"
#include "Enterprise/Input.h"
#include "Enterprise/Graphics.h"
#include "Enterprise/SceneManager.h"
#include "Enterprise/StateManager.h"
#include "Enterprise/GameEntryPoint.h"

namespace Enterprise
{

#ifdef EP_BUILD_DYNAMIC
bool Runtime::isEditor = false;
#endif // EP_BUILD_DYNAMIC
bool Runtime::isRunning = true;

std::unordered_map<HashName, std::vector<std::string>> Runtime::cmdLineOptions;
std::unordered_map<HashName, std::vector<HashName>> Runtime::cmdLineOptionSynonyms;
std::unordered_map<HashName, uint_fast16_t> Runtime::cmdLineOptionExpectedArgs;
std::vector<Runtime::cmdLineOptHelpEntry> Runtime::cmdLineOptionHelpRegistry;

void Runtime::Quit()
{
	isRunning = false;
}

void Runtime::RegisterCmdLineOption(std::string friendlyname,
									std::vector<std::string> options,
									std::string helpdescription,
									uint_fast16_t expectedArgCount)
{
	cmdLineOptionHelpRegistry.push_back
	(
		{
			friendlyname,
			helpdescription,
			options,
			expectedArgCount
		}
	);

	for (std::string& str : options)
	{
		EP_ASSERTF(cmdLineOptionSynonyms.count(HN(str)) == 0,
				   "Application: Duplicate registration for command-line option!");

		for (std::string& otherstr : options)
		{
			cmdLineOptionSynonyms[HN(str)].push_back(HN(otherstr));
			cmdLineOptionExpectedArgs[HN(str)] = expectedArgCount;
		}
	}
}

bool Runtime::CheckCmdLineOption(HashName option)
{
	EP_ASSERTF(cmdLineOptionSynonyms.count(option),
			   "Application: CheckCmdLineOption() invoked on unregistered "
			   "command-line option!");

	bool specified = false;
	for (HashName synonym : cmdLineOptionSynonyms[option])
	{
		if (cmdLineOptions.count(synonym))
		{
			if (!specified)
			{
				specified = true;
			}
			else
			{
				EP_WARN("Application: Multiple synonyms for command-line option \"{}\" "
						"were specified.", HN_ToStr(option));
			}
		}
	}

	return specified;
}

std::vector<std::string> Runtime::GetCmdLineOption(HashName option)
{
	EP_ASSERTF(cmdLineOptionSynonyms.count(option),
			   "Application: GetCmdLineOption() invoked on unregistered "
			   "command-line option!");
	EP_ASSERTF(cmdLineOptionExpectedArgs[option] > 0, 
			   "Application: GetCmdLineOption() invoked on a command-line "
			   "option that does not expect arguments!");

	std::vector<std::string> returnVal;
	bool specified = false;

	for (HashName& synonym : cmdLineOptionSynonyms[option])
	{
		if (cmdLineOptions.count(synonym))
		{
			if (!specified)
			{
				if (cmdLineOptions[synonym].size() == cmdLineOptionExpectedArgs[synonym])
				{
					specified = true;
					returnVal.insert(returnVal.begin(),
									 cmdLineOptions[synonym].begin(),
									 cmdLineOptions[synonym].end());
				}
				else
				{
					EP_ERROR("Application: Command-line option \"{}\" specified "
							 "with the wrong number of arguments.  "
							 "Expected: {}, Actual: {}",
							 HN_ToStr(synonym),
							 cmdLineOptionExpectedArgs[synonym], 
							 cmdLineOptions[synonym].size());
				}
			}
			else
			{
				EP_ERROR("Application: Multiple synonyms for command-line option \"{}\" "
						"were specified.", HN_ToStr(option));
			}
		}
	}

	return returnVal;
}

void Runtime::PrintCmdLineHelp()
{
	std::cout << "Enterprise" << ' ' << "0.0" << " Command Line Help" << std::endl;
	for (cmdLineOptHelpEntry& entry : cmdLineOptionHelpRegistry)
	{
		std::cout << "    " << entry.friendlyname << " (";
		for (auto synonymIt = entry.synonyms.begin();
			 synonymIt != entry.synonyms.end();
			 ++synonymIt)
		{
			std::cout << *synonymIt;
			if (synonymIt != entry.synonyms.end() - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << "): " << entry.helpdescription << std::endl;
	}
}

Runtime::Runtime()
{
	EP_ASSERT_NOREENTRY();

	#ifdef EP_CONFIG_DEBUG
	Console::Init();
	#endif

	RegisterCmdLineOption("Help", { "-h", "--help" }, 
						  "Displays command line options supported by this game.", 0);

	Events::Subscribe(HN("QuitRequested"), OnQuitRequested);

#ifdef EP_BUILD_DYNAMIC
	if (!isEditor) Window::Init();
#else
	Window::Init();
#endif // EP_BUILD_DYNAMIC

	File::Init();
	Input::Init();
	Graphics::Init();
	Time::Init();
	// Audio::Init();
	SceneManager::Init();

#ifndef EP_BUILD_DYNAMIC
	::GameSysInit();
	::GameInit();
#endif // EP_BUILD_DYNAMIC

	// Implement "--help" command line option
	if (CheckCmdLineOption(HN("-h")))
	{
		PrintCmdLineHelp();
		isRunning = false;
	}

	// TODO: Generate warnings for unused command line args
}

bool Runtime::Run()
{
	Time::Update();
	Input::Update();

	// Fixed update
	while (Time::ProcessFixedUpdate())
	{
#ifdef EP_BUILD_DYNAMIC
		if (isRunning)
		{
			SceneManager::FixedUpdate();
			StateManager::FixedUpdate();
		}
#else
		SceneManager::FixedUpdate();
		StateManager::FixedUpdate();
#endif
	}

	// Update
#ifdef EP_BUILD_DYNAMIC
	if(isRunning)
	{
		SceneManager::Update();
		StateManager::Update();
	}
#else
	SceneManager::Update();
	StateManager::Update();
#endif

	// Draw
#ifdef EP_BUILD_DYNAMIC
	if(!isEditor) Window::SwapBuffers();
#else
	Window::SwapBuffers();
#endif // EP_BUILD_DYNAMIC

	Graphics::PreDraw();
	SceneManager::PreDraw();

#ifdef EP_BUILD_DYNAMIC
	if (isRunning) StateManager::Draw();
#else
	StateManager::Draw();
#endif // EP_BUILD_DYNAMIC

	return isRunning;
}

Runtime::~Runtime()
{
#ifndef EP_BUILD_DYNAMIC
	::GameCleanup();
	::GameSysCleanup();
#endif // EP_BUILD_DYNAMIC

	StateManager::Cleanup();
	//SceneManager::Cleanup();

	Time::Cleanup();
	Graphics::Cleanup();

#ifdef EP_BUILD_DYNAMIC
	if (!isEditor) Window::Cleanup();
#else
	Window::Cleanup();
#endif // EP_BUILD_DYNAMIC

	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Cleanup();
	#endif
}

bool Runtime::OnQuitRequested(Events::Event& e)
{
	EP_ASSERT(e.Type() == HN("QuitRequested"));

	// By default, selecting Quit from the macOS dock or app menu quits the program.
	Enterprise::Runtime::Quit();
	return true;
}

}

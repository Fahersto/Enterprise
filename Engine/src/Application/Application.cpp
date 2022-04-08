#include "Enterprise/Core.h"

// Application stuff
#include "Enterprise/Application.h"

// Systems
#include "Enterprise/Time.h"
#include "Enterprise/File.h"
#include "Enterprise/Input.h"
#include "Enterprise/Graphics.h"
#include "Enterprise/Graphics/Window.h"
#include "Enterprise/SceneManager.h"
#include "Enterprise/StateManager.h"

#include "Enterprise/Core/Win32APIHelpers.h"

namespace Enterprise
{

// Static definitions
bool Application::_isRunning = true;
std::unordered_map<HashName, std::vector<std::string>> Application::_cmdLineOptions;
std::unordered_map<HashName, std::vector<HashName>> Application::_cmdLineOptionSynonyms;
std::unordered_map<HashName, uint_fast16_t> Application::_cmdLineOptionExpectedArgs;
std::vector<Application::cmdLineOptHelpEntry> Application::_cmdLineOptionHelpRegistry;


void Application::Quit()
{
	_isRunning = false;
}


void Application::RegisterCmdLineOption(std::string friendlyname,
										std::vector<std::string> options, 
										std::string helpdescription,
										uint_fast16_t expectedArgCount)
{
	_cmdLineOptionHelpRegistry.push_back
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
		EP_ASSERTF(_cmdLineOptionSynonyms.count(HN(str)) == 0,
				   "Application: Duplicate registration for command-line option!");

		for (std::string& otherstr : options)
		{
			_cmdLineOptionSynonyms[HN(str)].push_back(HN(otherstr));
			_cmdLineOptionExpectedArgs[HN(str)] = expectedArgCount;
		}
	}
}

bool Application::CheckCmdLineOption(HashName option)
{
	EP_ASSERTF(_cmdLineOptionSynonyms.count(option),
			   "Application: CheckCmdLineOption() invoked on unregistered "
			   "command-line option!");
	EP_ASSERTF(_cmdLineOptionExpectedArgs[option] == 0,
			   "Application: CheckCmdLineOption() invoked on a command-line "
			   "option that expects arguments!");

	bool specified = false;
	for (HashName synonym : _cmdLineOptionSynonyms[option])
	{
		if (_cmdLineOptions.count(synonym))
		{
			if (!specified)
			{
				specified = true;
				if (_cmdLineOptions[synonym].size() > 0)
				{
					EP_ERROR("Application: Unexpected non-option arguments detected "
							 "after command-line option \"{}\".", HN_ToStr(synonym));
				}
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

std::vector<std::string> Application::GetCmdLineOption(HashName option)
{
	EP_ASSERTF(_cmdLineOptionSynonyms.count(option),
			   "Application: GetCmdLineOption() invoked on unregistered "
			   "command-line option!");
	EP_ASSERTF(_cmdLineOptionExpectedArgs[option] > 0, 
			   "Application: GetCmdLineOption() invoked on a command-line "
			   "option that does not expect arguments!");

	std::vector<std::string> returnVal;
	bool specified = false;

	for (HashName& synonym : _cmdLineOptionSynonyms[option])
	{
		if (_cmdLineOptions.count(synonym))
		{
			if (!specified)
			{
				if (_cmdLineOptions[synonym].size() == _cmdLineOptionExpectedArgs[synonym])
				{
					specified = true;
					returnVal.insert(returnVal.begin(),
									 _cmdLineOptions[synonym].begin(),
									 _cmdLineOptions[synonym].end());
				}
				else
				{
					EP_ERROR("Application: Command-line option \"{}\" specified "
							 "with the wrong number of arguments.  "
							 "Expected: {}, Actual: {}",
							 HN_ToStr(synonym),
							 _cmdLineOptionExpectedArgs[synonym], 
							 _cmdLineOptions[synonym].size());
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

void Application::PrintCmdLineHelp()
{
	std::cout << "Enterprise" << ' ' << "0.0" << " Command Line Help" << std::endl;
	for (cmdLineOptHelpEntry& entry : _cmdLineOptionHelpRegistry)
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

Application::Application()
{
	EP_ASSERT_NOREENTRY();

	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Init();
	#endif

	RegisterCmdLineOption("Help", { "-h", "--help" }, 
						  "Displays command line options supported by this game.", 0);
	Events::Subscribe(HN("QuitRequested"), OnQuitRequested);

	File::Init();
	Input::Init();
	Graphics::Init();
	Time::Init();
	// Audio::Init();
	SceneManager::Init();

	// Implement "--help" command line option
	if (CheckCmdLineOption(HN("-h")))
	{
		PrintCmdLineHelp();
		_isRunning = false;
	}

	// TODO: Generate warnings for unused command line args
}

bool Application::Run()
{
	Time::Update();
	Input::Update();

	while (Time::ProcessFixedUpdate())
	{
		// Fixed update
		SceneManager::FixedUpdate();
		StateManager::FixedUpdate();
	}

	// Update
	SceneManager::Update();
	StateManager::Update();

	// Draw
	Window::SwapBuffers();
	Graphics::PreDraw();
	SceneManager::PreDraw();
	StateManager::Draw();

	return _isRunning;
}

Application::~Application()
{
	StateManager::Cleanup();
	//SceneManager::Cleanup();

	Time::Cleanup();
	Graphics::Cleanup();

	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Cleanup();
	#endif
}

bool Application::OnQuitRequested(Events::Event& e)
{
	EP_ASSERT(e.Type() == HN("QuitRequested"));

    // By default, selecting Quit from the macOS dock or app menu quits the program.
	Enterprise::Application::Quit();
	return true;
}

}

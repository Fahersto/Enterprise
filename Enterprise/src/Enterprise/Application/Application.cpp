#include "EP_PCH.h"
#include "Core.h"

// Application stuff
#include "Application.h"
#include "Game.h"

// Systems
#include "Enterprise/Time/Time.h"
#include "Enterprise/File/File.h"
#include "Enterprise/Input/Input.h"
#include "Enterprise/Graphics/Graphics.h"

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
	std::cout << Constants::AppName << ' ' << Constants::Version << " Command Line Help" << std::endl;
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
	Events::Subscribe(HN("QuitRequested"), OnQuit);

	// Initialize Systems
	Time::Init();
	File::Init();
	Input::Init();
	// Audio::Init();
	// ECS::Init();
	// StateStack::Init();
	Game::Init();
	Graphics::Init(); // Window creation must occur last.

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
	// Physics frame
	while (Time::PhysFrame())
	{
		// ...
	}

	// Frame
	Time::FrameStart();
	{
		Input::Update();
		Graphics::Update();
		// ...
	}
	Time::FrameEnd();

	return _isRunning;
}

Application::~Application()
{
	Graphics::Cleanup();
	Game::Cleanup();

	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Cleanup();
	#endif
}

bool Application::OnQuit(Events::Event& e)
{
	EP_ASSERT(e.Type() == HN("QuitRequested"));

    // By default, selecting Quit from the macOS dock or app menu quits the program.
	Enterprise::Application::Quit();
	return true;
}

}

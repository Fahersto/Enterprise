#include "EP_PCH.h"
#include "Core.h"

// Application stuff
#include "Application.h"
#include "Game.h"
#include "Window.h"
#include "Enterprise/Application/ApplicationEvents.h"

// Systems
#include "Enterprise/Time/Time.h"
#include "Enterprise/Input/Input.h"

namespace Enterprise 
{
bool Application::_isRunning = true;

void Application::Quit() { _isRunning = false; }

std::unordered_map<HashName, std::vector<std::string>> Application::_cmdLineOptions;
std::unordered_map<HashName, std::vector<HashName>> Application::_cmdLineOptionSynonyms;
std::unordered_map<HashName, uint_fast16_t> Application::_cmdLineOptionExpectedArgs;
std::vector<Application::cmdLineOptRegistryEntry> Application::_cmdLineOptionRegistry;

void Application::RegisterCmdLineOption(std::string friendlyname,
										std::string helpdescription,
										std::vector<std::string> options, 
										uint_fast16_t expectedArgCount)
{
	_cmdLineOptionRegistry.push_back
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

		// Populate each option's synonym lookup list.
		for (std::string& otherstr : options)
		{
			_cmdLineOptionSynonyms[HN(str)].push_back(HN(otherstr));
			_cmdLineOptionExpectedArgs[HN(str)] = expectedArgCount;
		}
	}
}

bool Application::CheckCmdLineOption(HashName opt)
{
	EP_ASSERTF(_cmdLineOptionSynonyms.count(opt),
			   "Application: CheckCmdLineOption() invoked on unregistered "
			   "command-line option!");
	EP_ASSERTF(_cmdLineOptionExpectedArgs[opt] == 0,
			   "Application: CheckCmdLineOption() invoked on a command-line "
			   "option that expects arguments!");

	bool specified = false;
	for (HashName synonym : _cmdLineOptionSynonyms[opt])
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
						"were specified.", HN_ToStr(opt));
			}
		}
	}

	return specified;
}

std::vector<std::string> Application::GetCmdLineOption(HashName opt)
{
	EP_ASSERTF(_cmdLineOptionSynonyms.count(opt),
			   "Application: GetCmdLineOption() invoked on unregistered "
			   "command-line option!");
	EP_ASSERTF(_cmdLineOptionExpectedArgs[opt] > 0, 
			   "Application: GetCmdLineOption() invoked on a command-line "
			   "option that does not expect arguments!");

	std::vector<std::string> returnVal;
	bool specified = false;

	for (HashName& synonym : _cmdLineOptionSynonyms[opt])
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
						"were specified.", HN_ToStr(opt));
			}
		}
	}

	return returnVal;
}


Application::Application()
{
	EP_ASSERT_NOREENTRY();

	// Create Console
	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Init();
	#endif

	// Initialize Systems
	Time::Init();
	// File::Init();
	// Network::Init();
	Input::Init();
	// Graphics::Init();
	// Audio::Init();
	// ECS::Init();
	// StateStack::Init();

	// Event subscriptions
	Events::SubscribeToType(EventTypes::WindowClose, OnEvent);
    Events::SubscribeToType(EventTypes::QuitRequested, OnEvent);

	Game::Init();

	// TODO: Assert if no window is created.

	//EP_ASSERT(false);
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
	Input::Update();
	// ...
	Time::FrameEnd();

	// Back to main function
	return _isRunning;
}

Application::~Application()
{
	Game::Cleanup();

	// Clean up the console
	#ifdef EP_CONFIG_DEBUG
	Enterprise::Console::Cleanup();
	#endif
}

bool Application::OnEvent(Events::Event& e)
{
    if (e.Type() == EventTypes::WindowClose)
        // By default, closing the window is treated as a request to quit.
        Enterprise::Events::Dispatch(EventTypes::QuitRequested);
    else if (e.Type() == EventTypes::QuitRequested)
        // By default, selecting Quit from the macOS dock or app menu quits the program.
		Enterprise::Application::Quit();
	return true;
}

}

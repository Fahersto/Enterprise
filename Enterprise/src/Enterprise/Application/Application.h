#pragma once
#include "Enterprise/Events/Events.h"

#if defined(__APPLE__) && defined (__MACH__)
int main(int argc, const char * argv[]);
#endif

namespace Enterprise
{
	/// The heart of an Enterprise program.  Instantiates and manages all of
	/// Enterprise's systems, and steps all engine and game code.
	class Application
	{
	public:
		/// Quit the application at the end of the current frame.
		static void Quit();

		/// Set up the application to handle a command line option.
		/// @param friendlyname The human-readable name of the option.
		/// @param helpdescription The description of the option to be displayed
		/// with -h or --help.
		/// @param options The list of option names that trigger this option. 
		/// All must start with "-".
		/// @param expectedArgCount The number of non-option arguments expected
		/// to follow this option.
		/// @remarks It is a best practice to use "-" for short names and "--"
		/// for long names, though anything that starts with "-" is a valid
		/// option argument.
		static void RegisterCmdLineOption(std::string friendlyname,
										  std::string helpdescription,
										  std::vector<std::string> options,
										  uint_fast16_t expectedArgCount);

		/// Check whether a command line option has been specified.
		/// @param option The HashName of the option to check, including hyphens.
		/// @return Returns true if the option was specified on the command
		/// line, otherwise returns false.
		/// @note This function accepts any synonym for @c option.
		/// @remarks Use this function only when handling an option that doesn't
		/// take additional arguments.  Use @c GetCmdLineOption() for options
		/// expecting arguments.
		static bool CheckCmdLineOption(HashName option);

		/// Get the non-option arguments associated with a command-line option.
		/// @param option The HashName of the option to check, including hyphens.
		/// @return A vector of the non-option arguments associated with the option.
		/// If the option was not specified, or if the wrong number of arguments
		/// were specified, an empty string will be returned.
		/// @note This function accepts any synonym for @c option.
		/// @remarks Use this function only when handling an option that expects
		/// additional arguments.  Use @c CheckCmdLineOption() to check whether
		/// a flag was specified.
		static std::vector<std::string> GetCmdLineOption(HashName option);

	private:
		#ifdef _WIN32
		friend int WINAPI ::WinMain(_In_ HINSTANCE hInstance,
									_In_opt_ HINSTANCE hPrevInstance,
									_In_ LPSTR lpCmdLine,
									_In_ int nCmdShow);
		#elif defined(__APPLE__) && defined(__MACH__)
		friend int ::main(int argc, const char* argv[]);
		#endif

		/// Set up the core systems of the application.
		Application();

		/// Step the entire Enterprise engine.
		/// @return Boolean indicating whether the main loop should continue.
		/// @remarks This function is called continuously by the main loop. Its
		/// return value is used as the condition for the loop.
		bool Run();

		/// Clean up the application prior to program termination.
		/// @note This destructor guaranteed to be called even in the case of
		/// unhandled exceptions.
		~Application();

		/// The return value used in @c Run().
		/// @remarks This value is set to false upon a call to @c Application::Quit().
		static bool _isRunning;

		/// Stores the list of command line options provided and their associated
		/// non-option arguments.
		static std::unordered_map<HashName, std::vector<std::string>> _cmdLineOptions;

		/// A map associating a command line option with its synonyms.
		static std::unordered_map<HashName, std::vector<HashName>> _cmdLineOptionSynonyms;

		/// A map associating a command line option with its number of expected arguments.
		static std::unordered_map<HashName, uint_fast16_t> _cmdLineOptionExpectedArgs;

		/// A full description of a supported command line option.
		struct cmdLineOptRegistryEntry
		{
			std::string friendlyname;
			std::string helpdescription;
			std::vector<std::string> synonyms;
			uint_fast16_t expectedArgCount;
		};

		/// A listing of all reigstered command line options.
		static std::vector<cmdLineOptRegistryEntry> _cmdLineOptionRegistry;

		/// Prints the description of registered command line options to the console.
		static void PrintCmdLineHelp();

		/// Application's event handler.
		/// @param e The event reference.
		/// @return Whether Application wishes to block the event from propogation.
		static bool OnEvent(Events::Event& e);
	};
}

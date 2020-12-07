#pragma once
#include "Enterprise/Events/Events.h"

namespace Enterprise
{
	/// The heart of an Enterprise program.  Instantiates and manages all of
	/// Enterprise's systems, and steps all engine and game code.
	/// @note Enterprise manages the application lifetime itself: do not try to
	/// instantiate an Application in game code.
	class Application
	{
	public:
		/// Quit the application at the end of the current frame.
		static void Quit();

		/// Set up the application to handle a command line option.
		/// @param friendlyname The human-readable name of the option.
		/// @param helpdescription The description of the option to be displayed
		/// with -h or --help.
		/// @param options A list of option arguments that triggers this option.
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
		/// @param opt The HashName of the option to check, including hyphens.
		/// @return Returns true if the option was specified on the command
		/// line, otherwise returns false.
		/// @note Both the long and short names of the option can be used for @copt.
		/// @remarks Use this function only when handling an option that doesn't
		/// take non-option arguments.  It is not necessary to check an option
		/// before using @cGetCmdOption() on it.
		static bool CheckCmdLineOption(HashName opt);

		/// Get the non-option argument associated with a command-line option.
		/// @param opt The HashName of the option to check, including hyphens.
		/// @return The first non-option argument following the option.  If no
		/// argument exists, an empty string will be returned.
		/// @note Both the long and short names of the option can be used for @copt.
		/// @note Enterprise will log a warning if the number of associated arguments
		/// does not equal 1.
		static std::vector<std::string> GetCmdLineOption(HashName opt);

		/// Get a list of the non-option arguments associated with a command-line
		/// option.
		/// @param opt The HashName of the option to check, including hyphens.
		/// @param count The number of expected non-option arguments.
		/// @return A vector of pointers to non-option arguments associated with the
		/// specified option.  If no associated arguments exist, the string will be
		/// empty.
		/// @note Both the long and short names of the option can be used for @copt.
		/// @note Enterprise will log a warning if the number of associated arguments
		/// does not equal @ccount.
		static std::vector<std::string> GetCmdLineOption(HashName opt, uint_fast8_t count);

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

		/// The return value used in @cRun().
		/// @remarks This value is set to false upon a call to @cApplication::Quit().
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

		/// Application's event handler.
		/// @param e The event reference.
		/// @return Whether Application wishes to block the event from propogation.
		static bool OnEvent(Events::Event& e);
	};
}

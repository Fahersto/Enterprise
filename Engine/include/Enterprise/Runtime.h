#pragma once
#include "Enterprise/Events.h"
#include "Enterprise/Runtime/GameModuleLoad.h"

// Needed for friending the Win32 entry point
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

// Needed for friending the macOS entry point
#if defined(__APPLE__) && defined (__MACH__)
int main(int argc, const char * argv[]);
#endif

// Needed for friending Launcher runtime
namespace Launcher { class Runtime; }

namespace Enterprise
{
	/// The heart of an Enterprise program.  Instantiates all of
	/// Enterprise's systems and steps all engine and game code.
	class EP_API Runtime
	{
	public:
		/// Quit the application at the end of the current frame.
		static void Quit();

		/// Set up the application to handle a command line option.
		/// @param friendlyname The human-readable name of the option.
		/// @param options The list of option names that trigger this option. 
		/// All must start with "-".
		/// @param helpdescription The description of the option to be displayed
		/// with -h or --help.
		/// @param expectedArgCount The number of non-option arguments expected
		/// to follow this option.
		/// @remarks It is a best practice to use "-" for short names and "--"
		/// for long names, though anything that starts with "-" is a valid
		/// option argument.
		static void RegisterCmdLineOption(std::string friendlyname,
										  std::vector<std::string> options,
										  std::string helpdescription,
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

		/// Get the arguments associated with a command-line option.
		/// @param option The HashName of the option to check, including hyphens.
		/// @return A vector containing the non-option arguments that followed
		/// the option. If the option was not specified, or if the wrong number
		/// of arguments were specified, an empty vector will be returned.
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

#ifdef EP_BUILD_DYNAMIC
		friend bool ::Enterprise::LoadGameModule(const std::string& projectFilePath);
		friend void ::Enterprise::UnloadGameModule();
		friend class ::Launcher::Runtime;
#endif

		friend class Window;
		friend class File;

#ifdef EP_BUILD_DYNAMIC
		static bool isEditor;
#endif
		static bool isRunning;
		static bool OnQuitRequested(Events::Event& e);

		static std::unordered_map<HashName, std::vector<std::string>> cmdLineOptions;
		static std::unordered_map<HashName, std::vector<HashName>> cmdLineOptionSynonyms;
		static std::unordered_map<HashName, uint_fast16_t> cmdLineOptionExpectedArgs;
		struct cmdLineOptHelpEntry
		{
			std::string friendlyname;
			std::string helpdescription;
			std::vector<std::string> synonyms;
			uint_fast16_t expectedArgCount;
		};
		static std::vector<cmdLineOptHelpEntry> cmdLineOptionHelpRegistry;
		static void PrintCmdLineHelp();

		Runtime();
		bool Run();
		~Runtime();
	};
}

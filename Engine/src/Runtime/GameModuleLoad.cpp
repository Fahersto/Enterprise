#ifdef EP_BUILD_DYNAMIC
#include "Enterprise/Runtime/GameModuleLoad.h"
#include "Enterprise/GameEntryPoint.h"
#include "Enterprise/Runtime.h"
#include "Enterprise/Core.h"
#include "Enterprise/File.h"

#include <filesystem>
#include <yaml-cpp/yaml.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "Enterprise/Core/Win32APIHelpers.h"
static HMODULE gameModuleHandle = NULL;
#else // macOS
static void* gameModuleHandle = NULL;
#endif

void Enterprise::LoadGameModule(const std::string& projectFilePath)
{
	std::string modulePath, moduleName;
	
	std::string ProjectFileContents;
	File::ErrorCode ec = File::LoadTextFile(projectFilePath, &ProjectFileContents);
	if (ec == File::ErrorCode::Success)
	{
		YAML::Node yamlIn = YAML::Load(ProjectFileContents);

		if (yamlIn.Type() == YAML::NodeType::Map)
		{
			if (yamlIn["ProjectName"])
			{
				moduleName = yamlIn["ProjectName"].as<std::string>();
			}
			else
			{
				EP_ERROR("LoadGameModule(): Project file does not contain \"ProjectName\" node!");
				return;
			}
			if (yamlIn["Bin"])
			{
				if (yamlIn["Bin"]["Debug"] &&
					yamlIn["Bin"]["Dev"] &&
					yamlIn["Bin"]["Release"] &&
					yamlIn["Bin"]["Editor"] &&
					yamlIn["Bin"]["Launcher"])
				{
					if (Runtime::isEditor)
					{
						modulePath = yamlIn["Bin"][yamlIn["Bin"]["Editor"].as<std::string>()].as<std::string>();
					}
					else
					{
						modulePath = yamlIn["Bin"][yamlIn["Bin"]["Launcher"].as<std::string>()].as<std::string>();
					}
					std::replace(modulePath.begin(), modulePath.end(), '\\', '/');
					if (modulePath.back() != '/')
						modulePath.append("/");
				}
				else
				{
					EP_ERROR("LoadGameModule(): Project file does not contain a complete \"Bin\" node!");
					return;
				}
			}
			else
			{
				EP_ERROR("LoadGameModule(): Project file does not contain \"Bin\" node!");
				return;
			}
		}
		else
		{
			EP_ERROR("LoadGameModule(): Project file's root node is not a map!");
			return;
		}
	}
	else
	{
		EP_ERROR("LoadGameModule(): Project file failed to open!  Error: {}", File::ErrorCodeToStr(ec));
		return;
	}

	std::string absProjectFilePath = std::filesystem::absolute(projectFilePath).remove_filename().string();
	std::replace(absProjectFilePath.begin(), absProjectFilePath.end(), '\\', '/');

	
	#ifdef _WIN32
	modulePath = absProjectFilePath + modulePath + moduleName + ".dll";
	gameModuleHandle = ::LoadLibrary(UTF8toWCHAR(modulePath).c_str());
	#else // macOS
	modulePath = absProjectFilePath + modulePath + "lib" + moduleName + ".dylib";
	// gameModuleHandle = ::dlopen();
	#endif
	
	if (gameModuleHandle)
	{
		#ifdef _WIN32
		::GameSysInit_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "SystemInit");
		::GameSysCleanup_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "SystemCleanup");
		::GameInit_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "GameInit");
		::GameCleanup_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "GameCleanup");
		::PieInit_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "PieInit");
		::PieCleanup_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "PieCleanup");
		#else // macOS
		// ::GameSysInit_Ptr = (void(*)())::dlsym(gameModuleHandle, "SystemInit");
		// ::GameSysCleanup_Ptr = (void(*)())::dlsym(gameModuleHandle, "SystemCleanup");
		// ::GameInit_Ptr = (void(*)())::dlsym(gameModuleHandle, "GameInit");
		// ::GameCleanup_Ptr = (void(*)())::dlsym(gameModuleHandle, "GameCleanup");
		// ::PieInit_Ptr = (void(*)())::dlsym(gameModuleHandle, "PieInit");
		// ::PieCleanup_Ptr = (void(*)())::dlsym(gameModuleHandle, "PieCleanup");
		#endif

		if (!::GameSysInit_Ptr ||
			!::GameSysCleanup_Ptr ||
			!::GameInit_Ptr ||
			!::GameCleanup_Ptr ||
			!::PieInit_Ptr ||
			!::PieCleanup_Ptr)
		{
			#ifdef _WIN32
			EP_ERROR("LoadGameModule(): Error loading entry point functions!  Error: {}", Win32_LastErrorMsg());
			::FreeLibrary(gameModuleHandle);
			#else // macOS
			// ::dlclose()
			#endif

			::GameSysInit_Ptr = nullptr;
			::GameSysCleanup_Ptr = nullptr;
			::GameInit_Ptr = nullptr;
			::GameCleanup_Ptr = nullptr;
			::PieInit_Ptr = nullptr;
			::PieCleanup_Ptr = nullptr;
			gameModuleHandle = NULL;
		}
	}
	else
	{
		#ifdef _WIN32
		EP_ERROR("LoadGameModule(): Error loading game module!  Error: {}", Win32_LastErrorMsg());
		#else
		// TODO: Report error on macOS
		#endif
	}
}

void Enterprise::UnloadGameModule()
{
	if (gameModuleHandle)
	{
		#ifdef _WIN32
		::FreeLibrary(gameModuleHandle);
		#else // macOS
		// ::dlclose()
		#endif
	}
}

#endif // EP_BUILD_DYNAMIC

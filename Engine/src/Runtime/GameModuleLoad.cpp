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
#include <dlfcn.h>
static void* gameModuleHandle = NULL;
#endif

bool Enterprise::LoadGameModule(const std::string& projectFilePath)
{
	// Confirm file ends in .epproj
	if (projectFilePath.length() > 7)
	{
		if (projectFilePath.compare(projectFilePath.length() - 7, 7, ".epproj") != 0)
		{
			EP_ERROR("LoadGameModule(): \"{}\" is not a .epproj file!", projectFilePath);
			return false;
		}
	}
	else
	{
		EP_ERROR("LoadGameModule(): \"{}\" is not a .epproj file!", projectFilePath);
		return false;
	}

	std::string projectFileContents, moduleName, modulePath;

	// Load project file
	File::ErrorCode ec = File::LoadTextFile(projectFilePath, &projectFileContents);
	if (ec == File::ErrorCode::Success)
	{
		try
		{
			YAML::Node yamlIn = YAML::Load(projectFileContents);

			if (yamlIn.Type() == YAML::NodeType::Map)
			{
				if (yamlIn["ProjectName"])
				{
					moduleName = yamlIn["ProjectName"].as<std::string>();
				}
				else
				{
					EP_ERROR("LoadGameModule(): Project file does not contain \"ProjectName\" key!");
					return false;
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
						EP_ERROR("LoadGameModule(): Project file's \"Bin\" node is incomplete!");
						return false;
					}
				}
				else
				{
					EP_ERROR("LoadGameModule(): Project file does not contain \"Bin\" node!");
					return false;
				}

				if (yamlIn["Directories"])
				{
					if (yamlIn["Directories"]["Content"] &&
						yamlIn["Directories"]["Data"] &&
						yamlIn["Directories"]["Save"] &&
						yamlIn["Directories"]["Temp"])
					{
						// Load paths
						File::contentDirPath = yamlIn["Directories"]["Content"].as<std::string>();
						File::dataDirPath = yamlIn["Directories"]["Data"].as<std::string>();
						File::saveDirPath = yamlIn["Directories"]["Save"].as<std::string>();
						File::tempDirPath = yamlIn["Directories"]["Temp"].as<std::string>();

						// Correct backslashes
						std::replace(File::contentDirPath.begin(), File::contentDirPath.end(), '\\', '/');
						std::replace(File::dataDirPath.begin(), File::dataDirPath.end(), '\\', '/');
						std::replace(File::saveDirPath.begin(), File::saveDirPath.end(), '\\', '/');
						std::replace(File::tempDirPath.begin(), File::tempDirPath.end(), '\\', '/');

						// Ensure paths end with slashes
						if (File::contentDirPath.back() != '/') File::contentDirPath.append("/");
						if (File::dataDirPath.back() != '/') File::dataDirPath.append("/");
						if (File::saveDirPath.back() != '/') File::saveDirPath.append("/");
						if (File::tempDirPath.back() != '/') File::tempDirPath.append("/");

						// Make paths relative to working directory
						size_t slashPos = projectFilePath.find_last_of('/');
						if (slashPos != std::string::npos)
						{
							std::string projectFileLocation = projectFilePath.substr(0, slashPos + 1);
							File::contentDirPath = projectFileLocation + File::contentDirPath;
							File::dataDirPath = projectFileLocation + File::dataDirPath;
							File::saveDirPath = projectFileLocation + File::saveDirPath;
							File::tempDirPath = projectFileLocation + File::tempDirPath;
						}

						// Ensure paths exist
						std::error_code ec;
						std::filesystem::create_directories(File::contentDirPath, ec);
						if(ec)
						{
							EP_ERROR("LoadGameModule(): Unable to create game content folder!  Path: {}  Error: {}", File::contentDirPath, ec.message());
							File::contentDirPath.clear();
							File::dataDirPath = File::editorDataDirPath;
							File::saveDirPath.clear();
							File::tempDirPath = File::editorTempDirPath;
							return false;
						}
						std::filesystem::create_directories(File::dataDirPath, ec);
						if(ec)
						{
							EP_ERROR("LoadGameModule(): Unable to create game data folder!  Path: {}  Error: {}", File::dataDirPath, ec.message());
							File::contentDirPath.clear();
							File::dataDirPath = File::editorDataDirPath;
							File::saveDirPath.clear();
							File::tempDirPath = File::editorTempDirPath;
							return false;
						}
						std::filesystem::create_directories(File::saveDirPath, ec);
						if(ec)
						{
							EP_ERROR("LoadGameModule(): Unable to create game save folder!  Path: {}  Error: {}", File::saveDirPath, ec.message());
							File::contentDirPath.clear();
							File::dataDirPath = File::editorDataDirPath;
							File::saveDirPath.clear();
							File::tempDirPath = File::editorTempDirPath;
							return false;
						}
						std::filesystem::create_directories(File::tempDirPath, ec);
						if(ec)
						{
							EP_ERROR("LoadGameModule(): Unable to create game temp folder!  Path: {}  Error: {}", File::tempDirPath, ec.message());
							File::contentDirPath.clear();
							File::dataDirPath = File::editorDataDirPath;
							File::saveDirPath.clear();
							File::tempDirPath = File::editorTempDirPath;
							return false;
						}
					}
					else
					{
						EP_ERROR("LoadGameModule(): Project file's \"Directories\" node is incomplete!");
						return false;
					}
				}
				else
				{
					EP_ERROR("LoadGameModule(): Project file does not contain a \"Directories\" key!");
					return false;
				}
			}
			else
			{
				EP_ERROR("LoadGameModule(): Project file's root node is not a map!");
				return false;
			}
		}
		catch (const YAML::Exception& e)
		{
			EP_ERROR("LoadGameModule(): Unhandled YAML exception thrown!  Message: {}", e.msg);
			return false;
		}
	}
	else
	{
		EP_ERROR("LoadGameModule(): \"{}\" failed to open!  Error: {}", projectFilePath, File::ErrorCodeToStr(ec));
		return false;
	}

	std::string absProjectFilePath = std::filesystem::absolute(projectFilePath).remove_filename().string();
	std::replace(absProjectFilePath.begin(), absProjectFilePath.end(), '\\', '/');

	#ifdef _WIN32
	modulePath = absProjectFilePath + modulePath + moduleName + ".dll";
	gameModuleHandle = ::LoadLibrary(UTF8toWCHAR(modulePath).c_str());
	#else // macOS
	modulePath = absProjectFilePath + modulePath + "lib" + moduleName + ".so";
	gameModuleHandle = ::dlopen(modulePath.c_str(), RTLD_LAZY | RTLD_LOCAL);
	#endif
	
	if (gameModuleHandle)
	{
		#ifdef _WIN32
		::GameSysInit_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "GameSysInit");
		::GameSysCleanup_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "GameSysCleanup");
		::GameInit_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "GameInit");
		::GameCleanup_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "GameCleanup");
		::PieInit_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "PieInit");
		::PieCleanup_Ptr = (void(*)())::GetProcAddress(gameModuleHandle, "PieCleanup");
		#else // macOS
		::GameSysInit_Ptr = (void(*)())::dlsym(gameModuleHandle, "GameSysInit");
		::GameSysCleanup_Ptr = (void(*)())::dlsym(gameModuleHandle, "GameSysCleanup");
		::GameInit_Ptr = (void(*)())::dlsym(gameModuleHandle, "GameInit");
		::GameCleanup_Ptr = (void(*)())::dlsym(gameModuleHandle, "GameCleanup");
		::PieInit_Ptr = (void(*)())::dlsym(gameModuleHandle, "PieInit");
		::PieCleanup_Ptr = (void(*)())::dlsym(gameModuleHandle, "PieCleanup");
		#endif

		if (!::GameSysInit_Ptr ||
			!::GameSysCleanup_Ptr ||
			!::GameInit_Ptr ||
			!::GameCleanup_Ptr ||
			!::PieInit_Ptr ||
			!::PieCleanup_Ptr)
		{
			#ifdef _WIN32
			EP_ERROR("LoadGameModule(): Error loading entry point functions!  Last error: {}", Win32_LastErrorMsg());
			::FreeLibrary(gameModuleHandle);
			#else // macOS
			EP_ERROR("LoadGameModule(): Error loading entry point functions!  Last error: {}", dlerror());
			::dlclose(gameModuleHandle);
			#endif

			::GameSysInit_Ptr = nullptr;
			::GameSysCleanup_Ptr = nullptr;
			::GameInit_Ptr = nullptr;
			::GameCleanup_Ptr = nullptr;
			::PieInit_Ptr = nullptr;
			::PieCleanup_Ptr = nullptr;
			
			gameModuleHandle = NULL;
			
			File::contentDirPath.clear();
			File::dataDirPath = File::editorDataDirPath;
			File::saveDirPath.clear();
			File::tempDirPath = File::editorTempDirPath;

			return false;
		}
	}
	else
	{
		#ifdef _WIN32
		EP_ERROR("LoadGameModule(): Error loading game module!  Error: {}", Win32_LastErrorMsg());
		#else
		EP_ERROR("LoadGameModule(): Error loading game module!  Error: {}", dlerror());
		#endif

		File::contentDirPath.clear();
		File::dataDirPath = File::editorDataDirPath;
		File::saveDirPath.clear();
		File::tempDirPath = File::editorTempDirPath;
		return false;
	}

	return true;
}

void Enterprise::UnloadGameModule()
{
	::GameSysInit_Ptr = nullptr;
	::GameSysCleanup_Ptr = nullptr;
	::GameInit_Ptr = nullptr;
	::GameCleanup_Ptr = nullptr;
	::PieInit_Ptr = nullptr;
	::PieCleanup_Ptr = nullptr;

	if (gameModuleHandle)
	{
		#ifdef _WIN32
		::FreeLibrary(gameModuleHandle);
		#else // macOS
		::dlclose(gameModuleHandle);
		#endif
	}

	File::contentDirPath.clear();
	File::dataDirPath = File::editorDataDirPath;
	File::saveDirPath.clear();
	File::tempDirPath = File::editorTempDirPath;
}

#endif // EP_BUILD_DYNAMIC

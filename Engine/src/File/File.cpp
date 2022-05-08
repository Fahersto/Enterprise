#include "Enterprise/File.h"
#include "Enterprise/Runtime.h"
#include <yaml-cpp/yaml.h>

using Enterprise::File;
using Enterprise::Runtime;

std::string File::contentDirPath;
std::string File::dataDirPath;
std::string File::saveDirPath;
std::string File::tempDirPath;
std::string File::shaderHeadersPath;

#ifdef EP_BUILD_DYNAMIC
std::string File::editorContentDirPath;
std::string File::editorDataDirPath;
std::string File::editorTempDirPath;
#endif // EP_BUILD_DYNAMIC

bool File::isAlphanumeric(const std::string& str)
{
	for (const char& c : str)
	{
		if (c != '-' && c != '.' && c != '_' &&
			!(c >= 0x41 && c <= 0x5A) && !(c >= 0x61 && c <= 0x7A) && // Letters
			!(c >= 0x30 && c <= 0x39)) // Numbers
		{
			return false;
		}
	}

	return true;
}

bool File::isAlphanumeric(const std::string_view& str)
{
	for (const char& c : str)
	{
		if (c != '-' && c != '.' && c != '_' &&
			!(c >= 0x41 && c <= 0x5A) && !(c >= 0x61 && c <= 0x7A) && // Letters
			!(c >= 0x30 && c <= 0x39)) // Numbers
		{
			return false;
		}
	}

	return true;
}


std::string Enterprise::File::VirtualPathToNative(const std::string& path)
{
	if (path.rfind("c/", 0) == 0)
	{
		return contentDirPath + path.substr(2);
	}
	else if (path.rfind("d/", 0) == 0)
	{
		return dataDirPath + path.substr(2);
	}
	else if (path.rfind("s/", 0) == 0)
	{
		return saveDirPath + path.substr(2);
	}
#ifdef EP_BUILD_DYNAMIC
	else if (path.rfind("ec/", 0) == 0)
	{
		if (!Runtime::isEditor)
			EP_WARN("File::VirtualPathToNative(): Dereferencing \"ec/\" outside of editor!");
		return editorContentDirPath + path.substr(3);
	}
	else if (path.rfind("ed/", 0) == 0)
	{
		if (!Runtime::isEditor)
			EP_WARN("File::VirtualPathToNative(): Dereferencing \"ec/\" outside of editor!");
		return editorDataDirPath + path.substr(3);
	}
#endif // EP_BUILD_DYNAMIC
	else
	{
		return path;
	}
}


bool File::Exists(const std::string& path)
{
	std::error_code ec;
	bool returnVal = std::filesystem::exists(VirtualPathToNative(path), ec);
	if (ec)
	{
		EP_ERROR("File::Exists(): Error checking file!  File: \"{}\", "
			"Error: {}, {}", path, ec.value(), ec.message());
	}

	return returnVal;
}


File::ErrorCode File::TextFileReader::Open(const std::string& path)
{
	m_LineNo = 0;
	m_path = path;

	if (m_stream.is_open()) Close();
	m_stream.open(VirtualPathToNative(path), std::ios_base::in);

	if (m_stream.fail())
	{
		if (errno == EACCES)
			m_errorcode = ErrorCode::PermissionFailure;
		if (errno == ENOENT)
			m_errorcode = ErrorCode::DoesNotExist;
		else
			m_errorcode = ErrorCode::Unhandled;
	}
	else
	{
		m_errorcode = ErrorCode::Success;
	}

	return m_errorcode;
}

void File::TextFileReader::Close()
{
	if (m_stream.is_open())
	{
		m_stream.clear();
		m_stream.close();
		if (m_stream.fail())
		{
			char errormessage[1024];
			strerror_s(errormessage, errno);

			EP_ERROR("TextFileReader::Close(): Error closing file!  "
				"File: \"{}\", Error: {}, {}", m_path, errno, errormessage);
		}
	}
}


std::string File::TextFileReader::GetLine()
{
	std::string returnVal;
	returnVal.reserve(250);

	if (m_stream.eof())
	{
		EP_WARN("TextFileReader::GetLine(): Attempted to read beyond EOF!  File: {}", m_path);
	}
	else
	{
		std::getline(m_stream, returnVal);
		m_LineNo++;

		if (!m_stream.eof() && m_stream.fail())
		{
			char errormessage[1024];
			strerror_s(errormessage, errno);

			EP_ERROR("TextFileReader::GetLine(): Read failure detected!  File will be closed.  "
				"Line: {} Error: {}, {}", m_LineNo, errno, errormessage);
			Close();
		}
	}
	return returnVal;
}


File::ErrorCode File::TextFileWriter::Open(const std::string& path)
{
	m_destinationFileNativePath = VirtualPathToNative(path);
	m_tempFileNativePath = GetNewTempFilename();

	if (m_stream.is_open()) Close();
	m_stream.open(m_tempFileNativePath, std::ios_base::out);

	if (m_stream.fail())
	{
		if (errno == EACCES)
			m_errorcode = ErrorCode::PermissionFailure;
		if (errno == ENOENT)
			m_errorcode = ErrorCode::DoesNotExist;
		else
			m_errorcode = ErrorCode::Unhandled;
	}
	else
	{
		m_errorcode = ErrorCode::Success;
	}

	return m_errorcode;
}


void File::Init()
{
	Runtime::RegisterCmdLineOption
	(
		"Developer's Sandbox",
		{ "-s", "--sandbox" },
		"Redirect virtual paths to the Enterprise source tree.", 0
	);
	Runtime::RegisterCmdLineOption
	(
		"Project File",
		{ "-p", "--project" },
		"Specify an Enterprise project file to open.", 1
	);

#ifdef EP_BUILD_DYNAMIC
	// Set paths for the editor and launcher
	if (Runtime::CheckCmdLineOption(HN("--sandbox")))
	{
		editorContentDirPath = "Editor/content/";
		editorDataDirPath = "data/Editor/data/";
		editorTempDirPath = "data/Editor/temp/";
		shaderHeadersPath = "Engine/include_glsl/";

		tempDirPath = editorTempDirPath;

		std::error_code ec;
		std::filesystem::create_directories(editorDataDirPath, ec);
		if(ec)
		{
			EP_FATAL("File::Init(): Unable to create editor data folder!");
			EP_DEBUGBREAK();
			throw Exceptions::FatalError();
		}
		std::filesystem::create_directories(editorTempDirPath, ec);
		if(ec)
		{
			EP_FATAL("File::Init(): Unable to create editor temp folder!");
			EP_DEBUGBREAK();
			throw Exceptions::FatalError();
		}
	}
	else
	{
		SetPlatformPathsForEditor();
		SetEditorPathForShaderHeaders();
	}
#else
	// Set paths for standalone
	if (Runtime::CheckCmdLineOption(HN("--project")))
	{
		if (Runtime::CheckCmdLineOption(HN("--sandbox")))
			shaderHeadersPath = "Engine/include_glsl/";
		else
			SetEditorPathForShaderHeaders();

		std::vector<std::string> projectOptionArgs = Runtime::GetCmdLineOption(HN("--project"));
		if (projectOptionArgs.size())
		{
			// Get project file path
			std::string &projectFilePath = projectOptionArgs.front();
			std::replace(projectFilePath.begin(), projectFilePath.end(), '\\', '/');

			// Confirm file ends in .epproj
			if (projectFilePath.length() > 7)
			{
				if (projectFilePath.compare(projectFilePath.length() - 7, 7, ".epproj") != 0)
				{
					EP_ERROR("File::Init(): \"{}\" is not an EPPROJ file!  Paths will not be redirected.", projectFilePath);
					SetPlatformPathsForStandalone();
					return;
				}
			}
			else
			{
				EP_ERROR("File::Init(): \"{}\" is not an EPPROJ file!  Paths will not be redirected.", projectFilePath);
				SetPlatformPathsForStandalone();
				return;
			}

			std::string projectFileContents;

			// Load project file
			ErrorCode ec = LoadTextFile(projectFilePath, &projectFileContents);
			if (ec == ErrorCode::Success)
			{
				try
				{
					YAML::Node yamlIn = YAML::Load(projectFileContents);

					if (yamlIn.Type() == YAML::NodeType::Map)
					{
						if (yamlIn["Directories"])
						{
							if (yamlIn["Directories"]["Content"] &&
								yamlIn["Directories"]["Data"] &&
								yamlIn["Directories"]["Save"] &&
								yamlIn["Directories"]["Temp"])
							{
								// Load paths
								contentDirPath = yamlIn["Directories"]["Content"].as<std::string>();
								dataDirPath = yamlIn["Directories"]["Data"].as<std::string>();
								saveDirPath = yamlIn["Directories"]["Save"].as<std::string>();
								tempDirPath = yamlIn["Directories"]["Temp"].as<std::string>();

								// Correct backslashes
								std::replace(contentDirPath.begin(), contentDirPath.end(), '\\', '/');
								std::replace(dataDirPath.begin(), dataDirPath.end(), '\\', '/');
								std::replace(saveDirPath.begin(), saveDirPath.end(), '\\', '/');
								std::replace(tempDirPath.begin(), tempDirPath.end(), '\\', '/');

								// Ensure paths end with slashes
								if (contentDirPath.back() != '/') contentDirPath.append("/");
								if (dataDirPath.back() != '/') dataDirPath.append("/");
								if (saveDirPath.back() != '/') saveDirPath.append("/");
								if (tempDirPath.back() != '/') tempDirPath.append("/");

								// Make paths relative to working directory
								size_t slashPos = projectFilePath.find_last_of('/');
								if (slashPos != std::string::npos)
								{
									projectFilePath.resize(slashPos + 1);
									contentDirPath = projectFilePath + contentDirPath;
									dataDirPath = projectFilePath + dataDirPath;
									saveDirPath = projectFilePath + saveDirPath;
									tempDirPath = projectFilePath + tempDirPath;
								}

								// Ensure paths exist
								std::error_code ec;
								std::filesystem::create_directories(contentDirPath, ec);
								if (ec)
								{
									EP_ERROR("File::Init(): Unable to create game content folder!  "
											 "Paths will not be redirected.  Path: {}  Error: {}",
											 contentDirPath, ec.message());
									SetPlatformPathsForStandalone();
									return;
								}
								std::filesystem::create_directories(dataDirPath, ec);
								if (ec)
								{
									EP_ERROR("File::Init(): Unable to create game data folder!  "
											 "Paths will not be redirected.  Path: {}  Error: {}",
											 dataDirPath, ec.message());
									SetPlatformPathsForStandalone();
									return;
								}
								std::filesystem::create_directories(saveDirPath, ec);
								if (ec)
								{
									EP_ERROR("File::Init(): Unable to create game save folder!  "
											 "Paths will not be redirected.  Path: {}  Error: {}",
											 saveDirPath, ec.message());
									SetPlatformPathsForStandalone();
									return;
								}
								std::filesystem::create_directories(tempDirPath, ec);
								if (ec)
								{
									EP_ERROR("File::Init(): Unable to create game temp folder!  "
											 "Paths will not be redirected.  Path: {}  Error: {}",
											 tempDirPath, ec.message());
									SetPlatformPathsForStandalone();
									return;
								}
							}
							else
							{
								EP_ERROR("File::Init(): Project file's \"Directories\" node is "
										 "incomplete!  Paths will not be redirected.");
								SetPlatformPathsForStandalone();
							}
						}
						else
						{
							EP_ERROR("File::Init(): Project file does not contain a \"Directories\" "
									 "key!  Paths will not be redirected.");
							SetPlatformPathsForStandalone();
						}
					}
					else
					{
						EP_ERROR("File::Init(): Project file's root node is not a map!  Paths will "
								 "not be redirected.");
						SetPlatformPathsForStandalone();
					}
				}
				catch (const YAML::Exception &e)
				{
					EP_ERROR("File::Init(): Unhandled YAML exception thrown!  Paths will not be "
							 "redirected.  Message: {}", e.msg);
					SetPlatformPathsForStandalone();
				}
			}
			else
			{
				EP_ERROR("File::Init(): \"{}\" failed to open!  Paths will not be redirected.  "
						 "Error: {}", projectFilePath, File::ErrorCodeToStr(ec));
				SetPlatformPathsForStandalone();
			}
		}
		else
		{
			EP_ERROR("File::Init(): \"--project\" was specified without any arguments!  "
					 "Paths will not be redirected.");
			SetPlatformPathsForStandalone();
		}
	}
	else
	{
		SetPlatformPathsForStandalone();
	}
#endif

}

#include "Enterprise/File.h"
#include "Enterprise/Runtime.h"
#include <yaml-cpp/yaml.h>

using Enterprise::File;
using Enterprise::Runtime;

std::string File::contentDirPath;
std::string File::dataDirPath;
std::string File::saveDirPath;
std::string File::tempDirPath;
std::string File::engineShadersPath;

#ifdef EP_BUILD_DYNAMIC
std::string File::editorContentDirPath;
std::string File::editorDataDirPath;
std::string File::editorTempDirPath;
#endif // EP_BUILD_DYNAMIC

void File::backslashesToSlashes(std::string& str)
{
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if (*it == '\\')
		{
			*it = '/';
		}
	}
}

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

	if (Runtime::CheckCmdLineOption(HN("--sandbox")))
	{
		engineShadersPath = "Engine/include_glsl/";
#if EP_BUILD_DYNAMIC
		if (Runtime::isEditor)
		{
			editorContentDirPath = "Editor/content/";
			editorDataDirPath = "data/Editor/data/";
			editorTempDirPath = "data/Editor/temp/";

			std::error_code ec;
			std::filesystem::create_directories(editorDataDirPath, ec);
			if(ec)
			{
				EP_ERROR("File::Init(): Unable to create editor data folder!");
				EP_DEBUGBREAK();
			}
			std::filesystem::create_directories(editorTempDirPath, ec);
			if(ec)
			{
				EP_ERROR("File::Init(): Unable to create editor temp folder!");
				EP_DEBUGBREAK();
			}
		}
#endif // EP_BUILD_DYNAMIC
	}
	else
	{
		SetPlatformEnginePaths();
	}

	// Project file loading
	std::vector<std::string> projectOptionArgs;
	if (Runtime::CheckCmdLineOption(HN("--project")))
	{
		projectOptionArgs = Runtime::GetCmdLineOption(HN("--project"));
		if (projectOptionArgs.size())
		{
			std::string& projectFileLocation = projectOptionArgs.front();
			backslashesToSlashes(projectFileLocation);
			
			std::string projectYAML;
			ErrorCode ec = LoadTextFile(projectFileLocation, &projectYAML);
			if (ec == ErrorCode::Success)
			{
				try
				{
					YAML::Node yamlIn = YAML::Load(projectYAML);

					if (yamlIn.Type() != YAML::NodeType::Map)
					{
						EP_ERROR("File::Init(): Project file's root node is not a map!");
						EP_DEBUGBREAK();
					}
					else
					{
						if (yamlIn["Directories"])
						{
							if (yamlIn["Directories"]["Content"])
							{
								contentDirPath = yamlIn["Directories"]["Content"].as<std::string>();
								backslashesToSlashes(contentDirPath);

								if (contentDirPath.back() != '/')
									contentDirPath.append("/");
							}
							if (yamlIn["Directories"]["Data"])
							{
								dataDirPath = yamlIn["Directories"]["Data"].as<std::string>();
								backslashesToSlashes(dataDirPath);
								if (dataDirPath.back() != '/')
									dataDirPath.append("/");
							}
							if (yamlIn["Directories"]["Save"])
							{
								saveDirPath = yamlIn["Directories"]["Save"].as<std::string>();
								backslashesToSlashes(saveDirPath);
								if (saveDirPath.back() != '/')
									saveDirPath.append("/");
							}
							if (yamlIn["Directories"]["Temp"])
							{
								tempDirPath = yamlIn["Directories"]["Temp"].as<std::string>();
								backslashesToSlashes(tempDirPath);
								if (tempDirPath.back() != '/')
									tempDirPath.append("/");
							}
						}

						// Confirm file ends in .epproj
						if (projectFileLocation.length() > 7)
						{
							if (projectFileLocation.compare(projectFileLocation.length() - 7, 7, ".epproj") != 0)
							{
								EP_ERROR("File::Init(): Argument to \"--project\" is not an EPPROJ file!  Argument: {}", projectFileLocation);
								EP_DEBUGBREAK();
							}
						}
						else
						{
							EP_ERROR("File::Init(): Argument to \"--project\" is not an EPPROJ file!  Argument: {}", projectFileLocation);
							EP_DEBUGBREAK();
						}

						// Make path relative to working directory
						size_t slashPos = projectFileLocation.find_last_of('/');
						if (slashPos != std::string::npos)
						{
							projectFileLocation.resize(slashPos + 1);
							contentDirPath = projectFileLocation + contentDirPath;
							dataDirPath = projectFileLocation + dataDirPath;
							saveDirPath = projectFileLocation + saveDirPath;
							tempDirPath = projectFileLocation + tempDirPath;
						}

						std::error_code ec;
						std::filesystem::create_directories(contentDirPath, ec);
						if(ec)
						{
							EP_ERROR("File::Init(): Unable to create content folder!");
							EP_DEBUGBREAK();
						}
						std::filesystem::create_directories(dataDirPath, ec);
						if(ec)
						{
							EP_ERROR("File::Init(): Unable to create data folder!");
							EP_DEBUGBREAK();
						}
						std::filesystem::create_directories(saveDirPath, ec);
						if(ec)
						{
							EP_ERROR("File::Init(): Unable to create save folder!");
							EP_DEBUGBREAK();
						}
						std::filesystem::create_directories(tempDirPath, ec);
						if(ec)
						{
							EP_ERROR("File::Init(): Unable to create temp folder!");
							EP_DEBUGBREAK();
						}
					}

					if (contentDirPath.empty())
						contentDirPath = "content/";
					if (dataDirPath.empty())
						dataDirPath = "data/data/";
					if (saveDirPath.empty())
						saveDirPath = "data/save/";
					if (tempDirPath.empty())
						tempDirPath = "data/temp/";
				}
				catch (const YAML::Exception& e)
				{
					EP_ERROR("File::Init(): YAML exception thrown!  Message: {}", e.msg);
				}
			}
			else
			{
				EP_ERROR("File::Init(): Error loading project file {}!  Error: {}",
						 projectFileLocation, ErrorCodeToStr(ec));
				EP_DEBUGBREAK();
			}
		}
		else
		{
			// TODO: Output error onto the command line
			EP_ERROR("File::Init(): \"--sandbox\" was specified without any arguments!");
			EP_DEBUGBREAK();
		}
	}
	else
	{
		SetPlatformPaths();
	}
}

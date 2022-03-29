#include "Enterprise/File/File.h"
#include "Enterprise/Application/Application.h"

using Enterprise::File;
using Enterprise::Application;

std::string File::contentDirPath;
std::string File::dataDirPath;
std::string File::saveDirPath;
std::string File::tempDirPath;
std::string File::engineShadersPath;


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
	Application::RegisterCmdLineOption
	(
		"Content Directory", 
		{ "-c", "--content-dir" },
		"Set a custom location for the game's \"content\" directory.", 1
	);
	Application::RegisterCmdLineOption
	(
		"Data Directory",
		{ "-d", "--data-dir" },
		"Set a custom location for the game's data directories.", 1
	);
	Application::RegisterCmdLineOption
	(
		"Engine Shaders Directory",
		{ "-e", "--engineshaders-dir" },
		"Set a custom location for the game's \"engineshaders\" directory.", 1
	);

	std::vector<std::string> cmdLinePath;

	// Content directory
	cmdLinePath = Application::GetCmdLineOption(HN("--content-dir"));
	if (cmdLinePath.size())
	{
		backslashesToSlashes(cmdLinePath.front());

		if (cmdLinePath.front().back() != '/')
			contentDirPath = cmdLinePath.front() + '/';
		else
			contentDirPath = cmdLinePath.front();
	}
	else
	{
		SetPlatformContentPath();
	}

	// Data directories
	cmdLinePath = Application::GetCmdLineOption(HN("--data-dir"));
	if (cmdLinePath.size())
	{
		backslashesToSlashes(cmdLinePath.front());

		if (cmdLinePath.front().back() == '/')
		{
			dataDirPath = cmdLinePath.front() + "data" + "/";
			saveDirPath = cmdLinePath.front() + "save" + "/";
			tempDirPath = cmdLinePath.front() + "temp" + "/";
		}
		else
		{
			dataDirPath = cmdLinePath.front() + "/" + "data" + "/";
			saveDirPath = cmdLinePath.front() + "/" + "save" + "/";
			tempDirPath = cmdLinePath.front() + "/" + "temp" + "/";
		}

		std::error_code ec;
		std::filesystem::create_directories(dataDirPath, ec);
		EP_ASSERTF(!ec, "File::Init(): Unable to create application data path!");
		std::filesystem::create_directories(saveDirPath, ec);
		EP_ASSERTF(!ec, "File::Init(): Unable to create save data path!");
		std::filesystem::create_directories(tempDirPath, ec);
		EP_ASSERTF(!ec, "File::Init(): Unable to create temp data path!");
	}
	else
	{
		SetPlatformDataPaths();
	}

	// Engine shaders directory	
	cmdLinePath = Application::GetCmdLineOption(HN("--engineshaders-dir"));
	if (cmdLinePath.size())
	{
		backslashesToSlashes(cmdLinePath.front());

		if (cmdLinePath.front().back() != '/')
			engineShadersPath = cmdLinePath.front() + '/';
		else
			engineShadersPath = cmdLinePath.front();
	}
	else
	{
		SetPlatformEShadersPath();
	}
}

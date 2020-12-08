#include "EP_PCH.h"
#include "File.h"
#include "Enterprise/Application/Application.h"

using Enterprise::File;
using Enterprise::Application;

std::string File::contentDirPath;
std::string File::userDirPath;
std::string File::machineDirPath;
std::string File::saveDirPath;
std::string File::tempDirPath;


void File::BackslashesToSlashes(std::string& str)
{
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if (*it == '\\')
		{
			*it = '/';
		}
	}
}

void File::SlashesToBackslashes(std::string& str)
{
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if (*it == '\\')
		{
			*it = '/';
		}
	}
}


bool File::Exists(const std::string& path)
{
	FILE* handle;
	errno_t errorcode;

#ifdef _WIN32
	errorcode = fopen_s(&handle, path.c_str(), "r");
#else
	handle = fopen(path.c_str(), "r");
#endif

	if (errorcode == 0)
	{
		// File exists and is open
		fclose(handle);
		return true;
	}
	else if (errorcode != ENOENT)
	{
		// File exists, but won't open.
		return true;
	}
	else
	{
		// File does not exist.
		return false;
	}
}

File::ErrorCode File::TextFileReader::Open(const std::string& path)
{
	if (m_isHandleOpen)
	{
		this->Close();
	}

#ifdef _WIN32

	errno_t err = fopen_s(&m_handle, path.c_str(), "r");

	if (err == 0)
	{
		m_errorcode = ErrorCode::Success;
		m_LineNo = 0;
		m_isHandleOpen = true;
		m_EOF = false;
	}
	else
	{
		if (errno == EACCES)
		{
			// Permission Denied
			m_errorcode = ErrorCode::PermissionFailure;
		}
		if (errno == ENOENT)
		{
			// File does not exist
			m_errorcode = ErrorCode::DoesNotExist;
		}
		else
		{
			// Unhandled error
			m_errorcode = ErrorCode::Unhandled;
		}

		m_isHandleOpen = false;

		char errormessage[80];
		strerror_s(errormessage, err);
		EP_ERROR("File System: Error opening file. {}", errormessage);
	}

#else // macOS

	m_handle = fopen(path.c_str(), "r");

	if (m_handle)
	{
		m_errorcode = ErrorCode::Success;
		m_LineNo = 0;
		m_isHandleOpen = true;
		m_EOF = false;
	}
	else
	{
		if (errno == EACCES)
		{
			// Permission Denied
			m_errorcode = ErrorCode::PermissionFailure;
		}
		if (errno == ENOENT)
		{
			// File does not exist
			m_errorcode = ErrorCode::DoesNotExist;
		}
		else
		{
			// Unhandled error
			m_errorcode = ErrorCode::Unhandled;
		}

		m_isHandleOpen = false;
		EP_ERROR("File System: Error opening file. {}", strerror(errno));
	}

#endif


	return m_errorcode;
}

void File::TextFileReader::Close()
{
	if (m_isHandleOpen)
	{
		fclose(m_handle);
		m_isHandleOpen = false;
	}
}

std::string File::TextFileReader::ReadNextLine()
{
	EP_ASSERTF(!m_EOF, "File::TextFileReader::ReadNextLine() called when EOF has been reached.");

	std::string returnVal;
	char buffer[200] = { 0 };
	bool done = false;

	while (!done) // We loop to allow us to handle lines larger than the buffer size.
	{
		if (fgets(buffer, 200, m_handle) == NULL) // Read unsuccessful.
		{
			// TODO: Should this be fatal?
			if (ferror(m_handle))
			{
				EP_FATAL("File System: Error encountered reading line in open text file.");
//				EP_DEBUGBREAK();
				throw Exceptions::FatalError();
			}

			if (feof(m_handle)) { m_EOF = true; }

			done = true;
		}
		else // Read successful.
		{
			if (feof(m_handle)) { m_EOF = true; }

			returnVal.append(buffer);

			if (returnVal.size() != 0)
			{
				if (returnVal.back() == '\n')
				{
					// Get rid of trailing newline character
					returnVal.resize(returnVal.size() - 1);
					m_LineNo++;
					done = true;
				}
			}
		}
	}
	return returnVal;
}

void Enterprise::File::Init()
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

	std::vector<std::string> cmdLinePath;

	// Set CONTENT path
	cmdLinePath = Application::GetCmdLineOption(HN("--content-dir"));
	if (cmdLinePath.size())
	{
		// Custom path specified on command line
		BackslashesToSlashes(cmdLinePath.front());

		if (cmdLinePath.front().back() != '/')
		{
			contentDirPath = cmdLinePath.front() + '/';
		}
		else
		{
			contentDirPath = cmdLinePath.front();
		}
	}
	else
	{
		// Use default path for OS
		SetPlatformContentPath();
	}

	// Set USER, MACHINE, LOCAL, and TEMP paths
	cmdLinePath = Application::GetCmdLineOption(HN("--data-dir"));
	if (cmdLinePath.size())
	{
		BackslashesToSlashes(cmdLinePath.front());

		if (cmdLinePath.front().back() == '/')
		{
			userDirPath = cmdLinePath.front() + "user" + "/";
			machineDirPath = cmdLinePath.front() + "machine" + "/";
			saveDirPath = cmdLinePath.front() + "save" + "/";
			tempDirPath = cmdLinePath.front() + "temp" + "/";
		}
		else
		{
			userDirPath = cmdLinePath.front() + "/" + "user" + "/";
			machineDirPath = cmdLinePath.front() + "/" + "machine" + "/";
			saveDirPath = cmdLinePath.front() + "/" + "save" + "/";
			tempDirPath = cmdLinePath.front() + "/" + "temp" + "/";
		}
	}
	else
	{
		// Use default path for OS
		SetPlatformDataPaths();
	}
}

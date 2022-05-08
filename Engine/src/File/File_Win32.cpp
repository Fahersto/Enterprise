#ifdef _WIN32
#include <ShlObj.h>
#include "Enterprise/Runtime.h"
#include "Enterprise/File.h"
#include "Enterprise/Core/Win32APIHelpers.h"
using Enterprise::File;


// File system stuff

std::string File::GetNewTempFilename()
{
	// Local singleton: do not call delete on it!
	static std::wstring* tempDirPath_Wide = nullptr;
	if (!tempDirPath_Wide)
	{
		tempDirPath_Wide = new std::wstring(UTF8toWCHAR(tempDirPath));
		for (auto it = tempDirPath_Wide->begin(); it != tempDirPath_Wide->end(); ++it)
		{
			if (*it == L'/') { *it = L'\\'; }
		}
	}

	WCHAR tempFileNameOut[MAX_PATH];
	if (GetTempFileName(tempDirPath_Wide->c_str(), L"EP_", 0, tempFileNameOut) == 0)
	{
		EP_ERROR("File::GetNewTempFilename(): Error generating temporary filename!  "
			"Error: {}", Win32_LastErrorMsg());
	}

	return WCHARtoUTF8(tempFileNameOut);
}


File::ErrorCode File::LoadTextFile(const std::string& path, std::string* outString)
{
	std::string nativePath = VirtualPathToNative(path);

	FILE* fhandle = NULL;
	errno_t err = fopen_s(&fhandle, nativePath.c_str(), "r");

	if (err == 0)
	{
		fseek(fhandle, 0, SEEK_END);
		outString->resize(ftell(fhandle));
		rewind(fhandle);
		fread_s(&(*outString)[0], outString->size(), sizeof(char), outString->size(), fhandle);
		fclose(fhandle);
		outString->resize(outString->find_last_not_of('\0') + 1);
		return ErrorCode::Success;
	}
	else
	{
		char errormessage[1024];
		strerror_s(errormessage, err);
		EP_ERROR("File::LoadTextFile(): Error opening file. {}", errormessage);

		if (errno == EACCES)
			return ErrorCode::PermissionFailure;
		if (errno == ENOENT)
			return ErrorCode::DoesNotExist;
		else
			return ErrorCode::Unhandled;
	}
}

File::ErrorCode File::SaveTextFile(const std::string& path, const std::string& inString)
{
	std::string tempFileName = GetNewTempFilename();

	FILE* fhandle = NULL;
	errno_t err = fopen_s(&fhandle, tempFileName.c_str(), "w");

	if (err == 0)
	{
		fwrite(inString.data(), sizeof(char), inString.size(), fhandle);
		fclose(fhandle);
	}
	else
	{
		char errormessage[1024];
		strerror_s(errormessage, err);
		EP_ERROR("File::SaveTextFile(): Error opening file. {}", errormessage);

		if (errno == EACCES)
			return ErrorCode::PermissionFailure;
		if (errno == ENOENT)
			return ErrorCode::DoesNotExist;
		else
			return ErrorCode::Unhandled;
	}

	if (MoveFileEx(UTF8toWCHAR(tempFileName).c_str(),
				  UTF8toWCHAR(VirtualPathToNative(path)).c_str(),
				  MOVEFILE_REPLACE_EXISTING) == 0)
	{
		EP_ERROR("File::SaveTextFile(): Error moving temp file to \"{}\"!  "
			"Error: {}", VirtualPathToNative(path), Win32_LastErrorMsg());
		return ErrorCode::Unhandled;
	}

	return ErrorCode::Success;
}


void File::TextFileWriter::Close()
{
	if (m_stream.is_open())
	{
		m_stream.clear();
		m_stream.close();
		if (m_stream.fail())
		{
			EP_ERROR("TextFileWriter::Close(): Could not close the file!  File: {}", m_tempFileNativePath);
		}

		std::error_code ec;
		std::filesystem::create_directories(m_destinationFileNativePath.substr(0, m_destinationFileNativePath.find_last_of('/')), ec);
		if (ec)
		{
			EP_ERROR("TextFileWriter::Close(): Error creating intermediate directories to \"{}\"!  "
				"Temporary file remains at \"{}\".  Error: {}", 
				m_destinationFileNativePath, m_tempFileNativePath, ec.message());
			return;
		}

		if (MoveFileEx(UTF8toWCHAR(m_tempFileNativePath).c_str(), 
					  UTF8toWCHAR(m_destinationFileNativePath).c_str(), 
					  MOVEFILE_REPLACE_EXISTING) == 0)
		{
			EP_ERROR("TextFileWriter::Close(): Error moving temp file to \"{}\"!  "
				"Error: {}", m_destinationFileNativePath, Win32_LastErrorMsg());
		}
	}
}

void File::SetEditorPathForShaderHeaders()
{
	// TODO: Implement more robust editor installation location lookup
	
	WCHAR buffer[MAX_PATH];
	EP_VERIFY_NEQ(GetEnvironmentVariable(L"PROGRAMFILES", buffer, MAX_PATH), 0);
	shaderHeadersPath = WCHARtoUTF8(buffer);

	std::replace(shaderHeadersPath.begin(), shaderHeadersPath.end(), '\\', '/');
	shaderHeadersPath += "/Michael Martz/Enterprise/include_glsl/";
}

#ifdef EP_BUILD_DYNAMIC
void File::SetPlatformPathsForEditor()
{
	editorContentDirPath = "content/";

	WCHAR* widePath = NULL;

	EP_VERIFY_EQ(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &widePath), S_OK);
	editorDataDirPath = WCHARtoUTF8(widePath) + '/' + "Michael Martz" + '/' + "Enterprise" + '/';
	std::replace(editorDataDirPath.begin(), editorDataDirPath.end(), '\\', '/');

	WCHAR buffer[MAX_PATH];
	EP_VERIFY_NEQ(GetTempPath(MAX_PATH, buffer), 0);
	editorTempDirPath = WCHARtoUTF8(buffer);
	std::replace(editorTempDirPath.begin(), editorTempDirPath.end(), '\\', '/');

	tempDirPath = editorTempDirPath; // Default

	// TODO: Provide exact path in assertion messages when ASSERTF is fixed
	std::error_code ec;
	std::filesystem::create_directories(editorDataDirPath, ec);
	EP_ASSERTF(!ec, "File::SetPlatformPathsForEditor(): Unable to create editor data folder!");
}
#else // Static build
void File::SetPlatformPathsForStandalone()
{
	contentDirPath = "content/";
	shaderHeadersPath = "include_glsl/";

	// TODO: Come back and set developer name and app name from Project Properties

	// Data
	WCHAR* widePath = NULL;
	EP_VERIFY_EQ(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &widePath), S_OK);
	dataDirPath = WCHARtoUTF8(widePath) + '/' + "Michael Martz" + '/' + "TestGame" + '/';
	std::replace(dataDirPath.begin(), dataDirPath.end(), '\\', '/');

	// Save
	EP_VERIFY_EQ(SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT, NULL, &widePath), S_OK);
	saveDirPath = WCHARtoUTF8(widePath) + '/' + "TestGame" + '/';
	std::replace(saveDirPath.begin(), saveDirPath.end(), '\\', '/');

	// Temp
	WCHAR buffer[MAX_PATH];
	EP_VERIFY_NEQ(GetTempPath(MAX_PATH, buffer), 0);
	tempDirPath = WCHARtoUTF8(buffer);
	std::replace(tempDirPath.begin(), tempDirPath.end(), '\\', '/');

	// TODO: Provide exact path in assertion messages when ASSERTF is fixed
	std::error_code ec;
	std::filesystem::create_directories(contentDirPath, ec);
	EP_ASSERTF(!ec, "File::SetPlatformPathsForStandalone(): Unable to create content data path!");
	std::filesystem::create_directories(dataDirPath, ec);
	EP_ASSERTF(!ec, "File::SetPlatformPathsForStandalone(): Unable to create application data path!");
	std::filesystem::create_directories(saveDirPath, ec);
	EP_ASSERTF(!ec, "File::SetPlatformPathsForStandalone(): Unable to create save data path!");
}

#endif

#endif // _WIN32

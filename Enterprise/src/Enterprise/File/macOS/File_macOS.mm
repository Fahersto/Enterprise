#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/File/File.h"
using Enterprise::File;


std::string File::GetNewTempFilename()
{
	// TODO: Implement
	return std::string();
}


File::ErrorCode File::LoadTextFile(const std::string& path, std::string* outString)
{
	std::string nativePath = VPathToNativePath(path);

	FILE* fhandle = fopen(nativePath.c_str(), "r");
	if (fhandle)
	{
		fseek(fhandle, 0, SEEK_END);
		outString->resize(ftell(fhandle));
		rewind(fhandle);
		fread(&(*outString)[0], sizeof(char), outString->size(), fhandle);
		fclose(fhandle);

		// Convert CRLF to LF (not handled by fread on macOS)
		size_t charsToSkip = 0;
		for (size_t i = 0; i < outString->size(); i++)
		{
			if (i + charsToSkip + 1 < outString->size())
			{
				if ((*outString)[i + charsToSkip] == '\r' && (*outString)[i + charsToSkip + 1] == '\n')
				{
					charsToSkip++;
				}
				(*outString)[i] = (*outString)[i + charsToSkip];
			}
			else
			{
				outString->resize(i);
			}
		}

		return ErrorCode::Success;
	}
	else
	{
		char errormessage[1024];
		strerror_s(errormessage, errno);
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
	// TODO: Implement
	return ErrorCode::Null;
}


void File::TextFileWriter::Close()
{
	// TODO: Implement
}


void File::SetPlatformContentPath()
{
	CFBundleRef bundle = CFBundleGetMainBundle();
	EP_ASSERT(bundle);
	CFURLRef contentURL = CFBundleCopyResourcesDirectoryURL(bundle);
	EP_ASSERT(contentURL);
	UInt8 contentPath[1025];
	EP_VERIFY(CFURLGetFileSystemRepresentation(contentURL, true, contentPath, 1024));

	contentDirPath = std::string((char*)contentPath) + "/content/";

	CFRelease(contentURL);
}

void File::SetPlatformDataPaths()
{
	@autoreleasepool
	{
		// Save is a subfolder of this location.
		NSURL* userPathURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
																	inDomain:NSUserDomainMask
														   appropriateForURL:nil
																	  create:YES
																	   error:nil];
		NSURL* globalPathURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
																	   inDomain:NSLocalDomainMask
															  appropriateForURL:nil
																		 create:YES
																		  error:nil];

		userDirPath = std::string(userPathURL.path.UTF8String) + '/' + Constants::DeveloperName + '/' + Constants::AppName + '/';
		globalDirPath = std::string(globalPathURL.path.UTF8String) + '/' + Constants::DeveloperName + '/' + Constants::AppName + '/';
		saveDirPath = userDirPath + "save/";
		tempDirPath = "Plz replace with real path, soon, mmkay?"; // TODO: Get temp path

		// TODO: Provide exact path in assertion messages when ASSERTF is fixed
		std::error_code ec;
		std::filesystem::create_directories(userDirPath, ec);
		EP_ASSERTF(!ec, "File::SetPlatformDataPaths(): Unable to create user data path!");
		std::filesystem::create_directories(globalDirPath, ec);
		EP_ASSERTF(!ec, "File::SetPlatformDataPaths(): Unable to create global data path!");
		std::filesystem::create_directories(saveDirPath, ec);
		EP_ASSERTF(!ec, "File::SetPlatformDataPaths(): Unable to create save data path!");
	}
}

#endif // macOS

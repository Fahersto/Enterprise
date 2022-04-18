#if defined(__APPLE__) && defined(__MACH__)

#import <AppKit/AppKit.h>
#include "Enterprise/File.h"
using Enterprise::File;

std::string File::GetNewTempFilename()
{
	std::string returnVal = tempDirPath + "EP_XXXX.tmp";
	int fd = mkstemps(returnVal.data(), 4);

	if (fd != -1)
	{
		// success
		close(fd);
		return returnVal;
	}
	else
	{
		// failure
		char errormessage[1024];
		strerror_s(errormessage, errno);
		EP_ERROR("File::GetNewTempFilename(): Error generating filename!  Error: {}", errormessage);
		return std::string();
	}
}


File::ErrorCode File::LoadTextFile(const std::string& path, std::string* outString)
{
	std::string nativePath = VirtualPathToNative(path);

	FILE* fhandle = fopen(nativePath.c_str(), "r");
	if (fhandle)
	{
		fseek(fhandle, 0, SEEK_END);
		outString->resize(ftell(fhandle));
		rewind(fhandle);
		fread(&(*outString)[0], sizeof(char), outString->size(), fhandle);
		fclose(fhandle);

		// Convert CRLF to LF (not handled by fread on macOS)
		size_t i = 0, charsToSkip = 0;
		for ( ; i + charsToSkip + 1 < outString->size(); i++)
		{
			if ((*outString)[i + charsToSkip] == '\r' && (*outString)[i + charsToSkip + 1] == '\n')
			{
				charsToSkip++;
			}
			(*outString)[i] = (*outString)[i + charsToSkip];
		}
		outString->resize(i + 1);

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
	std::string tempFileName = GetNewTempFilename();

	FILE* fhandle = fopen(tempFileName.c_str(), "w");

	if (fhandle)
	{
		fwrite(inString.data(), sizeof(char), inString.size(), fhandle);
		fclose(fhandle);
	}
	else
	{
		char errormessage[1024];
		strerror_s(errormessage, errno);
		EP_ERROR("File::SaveTextFile(): Error opening file. {}", errormessage);

		if (errno == EACCES)
			return ErrorCode::PermissionFailure;
		if (errno == ENOENT)
			return ErrorCode::DoesNotExist;
		else
			return ErrorCode::Unhandled;
	}

	@autoreleasepool
	{

		NSURL * tempFileURL = [NSURL fileURLWithFileSystemRepresentation:tempFileName.c_str()
															 isDirectory:NO
														   relativeToURL:nil];
		NSURL * destFileURL = [NSURL fileURLWithFileSystemRepresentation:VirtualPathToNative(path).c_str()
															 isDirectory:NO
														   relativeToURL:nil];

		NSError *error = nil;
		BOOL moveResult = [[NSFileManager defaultManager] replaceItemAtURL:destFileURL
															 withItemAtURL:tempFileURL
															backupItemName:nil
																   options:NSFileManagerItemReplacementUsingNewMetadataOnly
														  resultingItemURL:nil
																	 error:&error];

		if (moveResult == NO)
		{
			std::string errorDesc = error.localizedDescription.UTF8String;
			EP_ERROR("File::SaveTextFile(): Error moving temp file to \"{}\"!  "
					 "Error: {}, {}", VirtualPathToNative(path), error.code, errorDesc);

			if (error.code == EACCES)
				return ErrorCode::PermissionFailure;
			if (error.code == ENOENT)
				return ErrorCode::DoesNotExist;
			else
				return ErrorCode::Unhandled;
		}
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

		@autoreleasepool
		{
			NSURL * tempFileURL = [NSURL fileURLWithFileSystemRepresentation:m_tempFileNativePath.c_str()
																 isDirectory:NO
															   relativeToURL:nil];
			NSURL * destFileURL = [NSURL fileURLWithFileSystemRepresentation:m_destinationFileNativePath.c_str()
																 isDirectory:NO
															   relativeToURL:nil];

			std::error_code ec;
			std::filesystem::create_directories(m_destinationFileNativePath.substr(0, m_destinationFileNativePath.find_last_of('/')), ec);
			if (ec)
			{
				EP_ERROR("TextFileWriter::Close(): Error creating intermediate directories to \"{}\"!  "
						 "Temporary file remains at \"{}\".  Error: {}",
						 m_destinationFileNativePath, m_tempFileNativePath, ec.message());
				return;
			}

			NSError *error = nil;
			BOOL moveResult = [[NSFileManager defaultManager] replaceItemAtURL:destFileURL
																 withItemAtURL:tempFileURL
																backupItemName:nil
																	   options:NSFileManagerItemReplacementUsingNewMetadataOnly
															  resultingItemURL:nil
																		 error:&error];

			if (moveResult == NO)
			{
				std::string errorDesc = error.localizedDescription.UTF8String;
				EP_ERROR("TextFileWriter::Close(): Error moving temp file to \"{}\"!  "
						 "Error: {}, {}", m_destinationFileNativePath, error.code, errorDesc);
			}
		}
	}
}


void File::SetPlatformPaths()
{
	// Content
	CFBundleRef bundle = CFBundleGetMainBundle();
	EP_ASSERT(bundle);
	CFURLRef contentURL = CFBundleCopyResourcesDirectoryURL(bundle);
	EP_ASSERT(contentURL);
	UInt8 contentPath[1025];
	EP_VERIFY(CFURLGetFileSystemRepresentation(contentURL, true, contentPath, 1024));

	contentDirPath = std::string((char*)contentPath) + "/content/";

	CFRelease(contentURL);

	// Data
	@autoreleasepool
	{
		// Save is a subfolder of this location.
		NSURL* dataPathURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
																	inDomain:NSUserDomainMask
														   appropriateForURL:nil
																	  create:YES
																	   error:nil];
		NSURL* tempPathURL = [[NSFileManager defaultManager] URLForDirectory:NSItemReplacementDirectory
																	inDomain:NSUserDomainMask
														   appropriateForURL:dataPathURL
																	  create:YES
																	   error:nil];

		dataDirPath = std::string(dataPathURL.path.UTF8String) + '/' + "Constants::DeveloperName" + '/' + "Constants::AppName" + "/data/";
		saveDirPath = std::string(dataPathURL.path.UTF8String) + '/' + "Constants::DeveloperName" + '/' + "Constants::AppName" + "/save/";
		tempDirPath = std::string(tempPathURL.path.UTF8String) + '/';

		// TODO: Provide exact path in assertion messages when ASSERTF is fixed
		std::error_code ec;
		std::filesystem::create_directories(dataDirPath, ec);
		EP_ASSERTF(!ec, "File::SetPlatformDataPaths(): Unable to create application data path!");
		std::filesystem::create_directories(saveDirPath, ec);
		EP_ASSERTF(!ec, "File::SetPlatformDataPaths(): Unable to create save data path!");
	}
}

void File::SetPlatformEnginePaths()
{
	CFBundleRef bundle = CFBundleGetMainBundle();
	EP_ASSERT(bundle);
	CFURLRef contentURL = CFBundleCopyResourcesDirectoryURL(bundle);
	EP_ASSERT(contentURL);
	UInt8 contentPath[1025];
	EP_VERIFY(CFURLGetFileSystemRepresentation(contentURL, true, contentPath, 1024));

	engineShadersPath = std::string((char*)contentPath) + "/include_glsl/";

	CFRelease(contentURL);
}

#endif // macOS

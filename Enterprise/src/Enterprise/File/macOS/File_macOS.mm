#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/File/File.h"

void Enterprise::File::SetPlatformContentPath()
{
	CFBundleRef bundle = CFBundleGetMainBundle();
	EP_ASSERT(bundle);
	CFURLRef contentURL = CFBundleCopyResourcesDirectoryURL(bundle);
	EP_ASSERT(contentURL);
	UInt8 contentPath[1025];
	EP_VERIFY(CFURLGetFileSystemRepresentation(contentURL, true, contentPath, 1024));

	contentDirPath = std::string((char*)contentPath) + "/content/";

	CFRelease(contentURL);
	CFRelease(bundle);
}

void Enterprise::File::SetPlatformDataPaths()
{
	@autoreleasepool
	{
		// Save is a subfolder of this location.
		NSURL* userPathURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
																	inDomain:NSUserDomainMask
														   appropriateForURL:nil
																	  create:YES
																	   error:nil];
		NSURL* machinePathURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
																	   inDomain:NSLocalDomainMask
															  appropriateForURL:nil
																		 create:YES
																		  error:nil];

		userDirPath = std::string(userPathURL.path.UTF8String) + '/' + Constants::DeveloperName + '/' + Constants::AppName + '/';
		saveDirPath = userDirPath + "save/";
		machineDirPath = std::string(machinePathURL.path.UTF8String) + '/' + Constants::DeveloperName + '/' + Constants::AppName + '/';
		tempDirPath = "Plz replace with real path, soon, mmkay?"; // TODO: Either implement temp path or remove from VFS.
	}
}

#endif // macOS

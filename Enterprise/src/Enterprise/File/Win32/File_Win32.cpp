#include "EP_PCH.h"
#ifdef _WIN32

#include "Enterprise/File/File.h"

/// Helper function to convert from Windows wide paths to UTF-8.
/// @param widestring Wide-character C string
/// @return An std::string conversion into UTF-8
static std::string WCHARtoUTF8(WCHAR* widestring)
{
	size_t widestring_len = wcslen(widestring);
	
	if (widestring_len == 0)
	{
		return std::string();
	}

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, widestring, widestring_len, NULL, 0, NULL, NULL);
	std::string returnVal(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, widestring, widestring_len, &returnVal[0], size_needed, NULL, NULL);

	return returnVal;
}


void Enterprise::File::SetPlatformContentPath()
{
	contentDirPath = "content/";
}

void Enterprise::File::SetPlatformDataPaths()
{
	WCHAR* widePath = NULL;

	EP_VERIFY_EQ(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &widePath), S_OK);
	userDirPath = WCHARtoUTF8(widePath) + '/' + Constants::DeveloperName + '/' + Constants::AppName + '/';
	BackslashesToSlashes(userDirPath);

	EP_VERIFY_EQ(SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_DEFAULT, NULL, &widePath), S_OK);
	machineDirPath = WCHARtoUTF8(widePath) + '/' + Constants::DeveloperName + '/' + Constants::AppName + '/';
	BackslashesToSlashes(machineDirPath);

	EP_VERIFY_EQ(SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT, NULL, &widePath), S_OK);
	saveDirPath = WCHARtoUTF8(widePath) + '/' + Constants::AppName + '/';
	BackslashesToSlashes(saveDirPath);

	WCHAR buffer[MAX_PATH];
	EP_VERIFY_NEQ(GetTempPath(MAX_PATH, buffer), 0);
	tempDirPath = WCHARtoUTF8(buffer);
	BackslashesToSlashes(tempDirPath);
}

#endif // Win32

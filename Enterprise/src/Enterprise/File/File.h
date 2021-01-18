#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

namespace Constants
{
/// The name of this game's developer.  Used in file paths.
extern const char* DeveloperName;
/// The name of the game.  Used in file paths.
extern const char* AppName;
/// The version number of the game.  Used in command line help.
extern const char* Version;
}

/// The Enterprise file system.
class File
{
public:

	/// Convert an Enterprise VFS path to a native path.
	/// @param path Path in Enterprise VFS format.
	/// @return Path which can be used in native file system functions.
	static std::string VPathToNativePath(const std::string& path);

	/// The outcome of a file operation.
	enum class ErrorCode
	{
		Success,
		NotInitialized,
		PermissionFailure,
		DoesNotExist,
		Unhandled
	};

	/// Converts a File::ErrorCode to a human-readable string.
	static inline const char* ErrorCodeToString(ErrorCode err)
	{
		switch (err)
		{
			case ErrorCode::Success:
				return "File operation was successful.";
				break;
			case ErrorCode::NotInitialized:
				return "File operation attempted with uninitialized object.";
				break;
			case ErrorCode::PermissionFailure:
				return "Permission failure.";
				break;
			case ErrorCode::DoesNotExist:
				return "File does not exist";
				break;
			default:
				return "Unhandled error!";
				break;
		}
	}

	/// Returns true if the file at the given path exists.
	/// @param path The path to check.
	/// @return True if the file exists.
	static bool Exists(const std::string& path);


	/// Opens a handle to a text file and provides methods to read data from it.
	class TextFileReader
	{
	public:
		TextFileReader() {};
		/// Open a file handle during construction.
		/// @param path The path the text file.
		TextFileReader(const std::string& path) { m_errorcode = Open(path); }
		~TextFileReader() { Close(); }

		/// Open a new text file.
		/// @param path The path to the text file.
		/// @return Result of the file open operation.
		/// @note If this reader already has a file open, this call will automatically close it.
		ErrorCode Open(const std::string& path);
		/// Close this reader's open file handle.
		/// @note Calling this function when no file handle is open causes no ill effects.
		void Close();
		/// Get the result of the last file open operation.
		ErrorCode GetError() { return m_errorcode; }
		
		/// Get the next line of the text file, and increments the position in the file.
		std::string ReadNextLine();
		/// Get the current line number of the reader.
		/// @note The return value of this call is the number of the last read line.  A value of
		/// 0 means that no lines have been read yet.
		size_t CurrentLine() { return m_LineNo; }

		/// Returns true if the end of the file has been reached.
		bool isEOF() { return m_EOF; }

	private:
		FILE* m_handle = NULL;
		ErrorCode m_errorcode = ErrorCode::NotInitialized;
		unsigned int m_LineNo = 0;
		bool m_isHandleOpen = false;
		bool m_EOF = false;
	};


	/// Converts a string from INI value format to a boolean.
	/// @param str The string to convert.
	/// @return First: 'true' if conversion was successful. Second: The converted value.
	static std::pair<bool, bool> INIStringToBool(const std::string& str);
	/// Converts a string from INI value format to an integer.
	/// @param str The string to convert.
	/// @return First: 'true' if conversion was successful. Second: The converted value.
	static std::pair<bool, int> INIStringToInt(const std::string& str);
	/// Converts a string from INI value format to a float.
	/// @param str The string to convert.
	/// @return First: 'true' if conversion was successful. Second: The converted value.
	static std::pair<bool, float> INIStringToFloat(const std::string& str);
	/// Converts a string from INI value format to a dictionary structure.
	/// @param str The string to convert.
	/// @return The converted dictionary.  If the conversion fails, this will be empty.
	static std::unordered_map<HashName, std::string> INIStringToDictionary(const std::string& str);


	/// A structure containing the loaded contents of an INI file, and methods for extracting that data.
	class INIReader
	{
	public:
		INIReader(const std::string& filename, bool areErrorsFatal, const std::string& section = "")
		{
			Load(filename, areErrorsFatal, section);
		}

		/// Opens a new INI file and populates this INIReader's internal buffer.
		/// @param filename Path to the file to load.
		/// @param areErrorsFatal If true, a failure to load the file will trigger a fatal error.
		/// @param section Stringname of the section, or section group, to include.
		/// @return Error/status code for the load operation.
		ErrorCode Load(const std::string& filename, bool areErrorsFatal, const std::string& section = "");

		/// Gets the error/status code of the previous Open() call.
		/// @remarks If Open() is called with areErrorsFatal == true, then this check is not needed.
		ErrorCode GetError() { return m_errorcode; }

		/// Gets a vector of HashNames for the INI sections loaded into this reader object.
		std::vector<HashName> Sections() { return m_sections; }

		/// Get a value from a given section and key as a boolean.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal The value to return if the key fails to load or parse.
		/// @return The retrieved value.
		bool				GetBool(HashName section, HashName key, bool defaultVal);
		/// Get the list of values of a given section and key as booleans.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return The list of retrieved values.
		std::vector<bool>	GetMultiBool(HashName section, HashName key);

		/// Get a value from a given section and key as an int.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal The value to return if the key fails to load or parse.
		/// @return The retrieved value.
		int					GetInt(HashName section, HashName key, int defaultVal);
		/// Get the list of values of a given section and key as ints.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return The list of retrieved values.
		std::vector<int>	GetMultiInt(HashName section, HashName key);

		/// Get a value from a given section and key as a float.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal The value to return if the key fails to load or parse.
		/// @return The retrieved value.
		float				GetFloat(HashName section, HashName key, float defaultVal);
		/// Get the list of values of a given section and key as floats.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return The list of retrieved values.
		std::vector<float>	GetMultiFloat(HashName section, HashName key);

		/// Get a value from a given section and key as a string.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal The value to return if the key fails to load or parse.
		/// @return The retrieved value.
		std::string					GetStr(HashName section, HashName key, std::string defaultVal);
		/// Get the list of values of a given section and key as strings.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return The list of retrieved values.
		std::vector<std::string>	GetMultiStr(HashName section, HashName key);

		/// Get a value from a given section and key as a dictionary.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal The value to return if the key fails to load or parse.
		/// @return The retrieved dictionary.
		std::unordered_map<HashName, std::string>				GetDictionary(HashName section, HashName key);
		/// Get the list of values of a given section and key as dictionaries.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal The value to return if the key fails to load or parse.
		/// @return The list of retrieved dictionaries.
		std::vector<std::unordered_map<HashName, std::string>>	GetMultiDictionary(HashName section, HashName key);

	private:
		// A list of the HashNames for all sections represented by this INIReader.
		std::vector<HashName> m_sections;
		// Associates a section/key HashName combination with the vector of all values as strings.
		std::unordered_map<HashName, std::unordered_map<HashName, std::vector<std::string>>> m_data;
		// The internally stored error/status code from the loading of the file.
		ErrorCode m_errorcode;
		// The virtual path to the file this reader represents.
		std::string m_path;
	};

private:
	friend class Application;

	static std::string contentDirPath;
	static std::string userDirPath;
	static std::string machineDirPath;
	static std::string saveDirPath;
	static std::string tempDirPath;

	/// Helper function which replaces any backslashes in a string with
	/// forward slashes.
	/// @param str Reference to the string to process.
	static void BackslashesToSlashes(std::string& str);
	/// Helper function which replaces any forward slashes in a string with
	/// backslashes.
	/// @param str Reference to the string to process.
	static void SlashesToBackslashes(std::string& str);

	/// Set contentDirPath to the default for this platform.
	static void SetPlatformContentPath();
	/// Set the paths to the data folders to the default for this platform.
	static void SetPlatformDataPaths();

	/// Sets up the File system.
	static void Init();
};

}

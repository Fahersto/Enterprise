#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise
{

/// The Enterprise file system.
class File
{
public:

	/// The outcome of a file operation.
	enum class ErrorCode
	{
		Success,
		NotInitialized,
		PermissionFailure,
		DoesNotExist,
		Unhandled
	};


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
	/// Converts a string from INI value format to an std::unordered_map.
	/// @param str The string to convert.
	/// @return First: 'true' if conversion was successful. Second: The converted dictionary.
	static std::pair<bool, std::unordered_map<HashName, std::string>>
		INIStringToDictionary(const std::string& str);


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
		/// @param defaultVal If the key fails to load or parse, the value to return.
		/// @return First: 'true' if get was successful. Second: Retrieved value.
		std::pair<bool, bool>				GetBool(HashName section, HashName key, bool defaultVal);
		/// Get the list of values of a given section and key as booleans.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return First: 'true' if get was successful. Second: List of retrieved values.
		std::pair<bool, std::vector<bool>>	GetMultiBool(HashName section, HashName key);

		/// Get a value from a given section and key as an int.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal If the key fails to load or parse, the value to return.
		/// @return First: 'true' if get was successful. Second: Retrieved value.
		std::pair<bool, int>				GetInt(HashName section, HashName key, int defaultVal);
		/// Get the list of values of a given section and key as ints.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return First: 'true' if get was successful. Second: List of retrieved values.
		std::pair<bool, std::vector<int>>	GetMultiInt(HashName section, HashName key);

		/// Get a value from a given section and key as a float.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal If the key fails to load or parse, the value to return.
		/// @return First: 'true' if get was successful. Second: Retrieved value.
		std::pair<bool, float>				GetFloat(HashName section, HashName key, float defaultVal);
		/// Get the list of values of a given section and key as floats.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return First: 'true' if get was successful. Second: List of retrieved values.
		std::pair<bool, std::vector<float>>	GetMultiFloat(HashName section, HashName key);

		/// Get a value from a given section and key as a string.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal If the key fails to load or parse, the value to return.
		/// @return First: 'true' if get was successful. Second: Retrieved value.
		std::pair<bool, std::string>				GetStr(HashName section, HashName key, std::string defaultVal);
		/// Get the list of values of a given section and key as strings.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @return First: 'true' if get was successful. Second: List of retrieved values.
		std::pair<bool, std::vector<std::string>>	GetMultiStr(HashName section, HashName key);

		/// Get a value from a given section and key as an std::unordered_map.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal If the key fails to load or parse, the value to return.
		/// @return First: 'true' if get was successful. Second: The value, converted to an std::unordered_map.
		std::pair<bool, std::unordered_map<HashName, std::string>>				GetDictionary(HashName section, HashName key);
		/// Get the list of values of a given section and key as std::unordered_maps.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to look up.
		/// @param defaultVal If the key fails to load or parse, the value to return.
		/// @return First: 'true' if get was successful. Second: List of retrieved values, converted to std::unordered_maps.
		std::pair<bool, std::vector<std::unordered_map<HashName, std::string>>>	GetMultiDictionary(HashName section, HashName key);

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
};

}

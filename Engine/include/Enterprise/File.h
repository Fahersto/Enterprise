#pragma once
#include "Enterprise/Core.h"
#include "Enterprise/File/INIValueType.h"
#include "Enterprise/Runtime/GameModuleLoad.h"

#include <fstream>
#include <set>
#include <map>
#include <sstream>

namespace Enterprise
{

/// The Enterprise file system.
class File
{
public:
	/// Convert a virtual path to a native path.
	/// @param path The virtual file path.
	/// @return The equivalent native file path.
	/// @note If the provided path does not start with a virtual drive letter ("c/", "u/", "g/", or "s/"),
	/// it is treated as a native path and is returned unmodified.  This does not generate a warning.
	EP_API static std::string VirtualPathToNative(const std::string& path);

	/// Check whether a file exists.
	/// @param path The virtual path of the file to check.
	/// @return @c true if the file exists.
	EP_API static bool Exists(const std::string& path);


	/// The outcome of a file operation.
	enum class ErrorCode
	{
		Success,
		PermissionFailure,
		DoesNotExist,
		Unhandled,
		Null
	};

	/// Convert an error code to a human-readable string.
	static inline const char* ErrorCodeToStr(ErrorCode err)
	{
		switch (err)
		{
		case ErrorCode::Success:
			return "Operation successful!";
			break;
		case ErrorCode::PermissionFailure:
			return "Permission failure.";
			break;
		case ErrorCode::DoesNotExist:
			return "The file does not exist.";
			break;
		case ErrorCode::Null:
			return "Null error code!";
			break;
		default:
			return "Unhandled error!";
			break;
		}
	}

	/// Load the contents of a text file into an std::string.
	/// @param path The virtual path of the text file.
	/// @param outString Pointer to the string object that will receive the file contents.
	/// @return The result of the file open operation.
	/// @remarks This method can be used to load an entire text file into memory at once.
	/// To stream larger files, use a File::TextFileReader instance instead.
	EP_API static ErrorCode LoadTextFile(const std::string& path, std::string* outString);
	/// Save the contents of a string to a text file.
	/// @param path The virtual path of the text file.
	/// @param inString String containing the text to save.
	/// @return The result of the file save operation.
	/// @note This function will replace any file that already exists at 'path'.
	/// @remarks This method can be used to dump a fully-formed text file from memory to a text file.
	/// To write discontiguous text to file, use TextFileWriter.
	EP_API static ErrorCode SaveTextFile(const std::string& path, const std::string& inString);

	/// A helper class for streaming data from text files.
	/// To use it, create a class instance, open a file with Open() and get each line of text with GetLine().
	class TextFileReader
	{
	public:
		TextFileReader() {};
		/// Optionally open a file during object construction.
		/// @param path The virtual path of the text file.
		/// @note To confirm the file was opened successfully, use GetLastError().
		TextFileReader(const std::string& path) { m_errorcode = Open(path); }
		~TextFileReader() { Close(); }

		/// Open a new text file.
		/// @param path The virtual path of the text file.
		/// @return Result of the file open operation.
		/// @note If invoked on an instance of TextFileReader that is already reading a file,
		/// this method will automatically close the first file handle.
		ErrorCode Open(const std::string& path);
		/// Get the result of the last file open operation.
		/// @return The result of the last file open operation.
		inline ErrorCode GetLastError() { return m_errorcode; }
		/// Close the currently open file.
		/// @note This method is invoked automatically by the class destructor.
		/// @note It is safe to call this method even when no file is open.
		void Close();
		
		/// Read the next line in the file.
		/// @return A string containing the contents of the next line in the file.
		std::string GetLine();
		/// Get the line number of the last read line.
		/// @return The line number of the last line returned by GetLine().
		/// @note A return value of 0 indicates that no lines have been read yet.
		inline unsigned int LineNo() { return m_LineNo; }
		/// Check whether the end of the file has been reached.
		/// @return @c true if the last line of the file has been read.
		inline bool isEOF() { return m_stream.eof(); }

	private:
		std::string m_path;
		std::ifstream m_stream;
		ErrorCode m_errorcode = ErrorCode::Null;
		unsigned int m_LineNo = 0;
	};


	/// A helper class for streaming data to text files.
	/// To use it, create a class instance, open a file with Open() and write text using operator <<.
	class TextFileWriter
	{
	public:
		TextFileWriter() {};
		/// Optionally open a file during object construction.
		/// @param path The virtual path of the text file.
		/// @note To confirm the file was opened successfully, use GetLastError().
		TextFileWriter(const std::string& path) { m_errorcode = Open(path); }
		~TextFileWriter() { Close(); }

		/// Open a new text file.
		/// @param path The virtual path of the text file.
		/// @return Result of the file open operation.
		/// @note If a file is already open, this method will automatically close it.
		ErrorCode Open(const std::string& path);
		/// Get the result of the last file open operation.
		/// @return The result of the last file open operation.
		inline ErrorCode GetLastError() { return m_errorcode; }
		/// Close the currently open file.
		/// @note This method is invoked automatically by ~TextFileWriter().
		/// @note It is safe to invoke this method even when no file is open.
		void Close();

		/// Output text to the file.
		template<typename T>
		inline std::enable_if_t<has_insertion_operator<T>::value, TextFileWriter&> operator<<(T data)
		{
			if (m_stream.is_open())
			{
				m_stream << data;
			}
			else
			{
				EP_ERROR("TextFileWriter::operator <<(): Attempted to write data with no file open!");
			}
			return *this;
		}

	private:
		std::string m_destinationFileNativePath, m_tempFileNativePath;
		std::ofstream m_stream;
		ErrorCode m_errorcode = ErrorCode::Null;
	};


	/// A helper class for reading data from INI files.
	/// To use it, create a class instance, load a file with Load() and use the Get methods to obtain values.
	class INIReader
	{
	public:
		INIReader() {};
		/// Optionally open an INI file during object construction.
		/// @param path The virtual path of the INI file.
		/// @param fallbackPath The virtual path of the fallback INI file.
		/// @param sectionFilters The list of sections or section groups to load.
		/// @note If no filters are provided, all sections are loaded.
		/// @note To confirm the file was opened successfully, use GetLastError().
		INIReader(const std::string& path, const std::string& fallbackPath = "",
			std::initializer_list<const std::string> sectionFilters = {})
		{
			Load(path, fallbackPath, sectionFilters);
		}
		/// Load a new INI file.
		/// @param path The virtual path of the INI file.
		/// @param fallbackPath The virtual path of the fallback INI file.
		/// @param sectionFilters The list of sections or section groups to load.
		/// @return The result of the open operation.
		/// @note If no filters are provided, all sections are loaded.
		/// @note If this INIReader was previously used to read an INI, its internal data store will be reset.
		/// @remarks If the INI file does not exist at 'path', the INI file at 'fallbackPath' will be copied to 'path'.
		/// This will not result in an unsuccessful error code.
		ErrorCode Load(const std::string& path, const std::string& fallbackPath = "",
			std::initializer_list<const std::string> sectionFilters = {});
		/// Get the result of the last file open operation.
		/// @return The result of the last file open operation.
		inline ErrorCode GetLastError() { return m_errorcode; }
		
		/// Get the HashNames of all loaded INI sections.
		/// @return The HashNames of every loaded INI section.
		/// @note If a section was loaded as part of a group filter, the section name will not precede the group name.
		inline const std::set<HashName>& Sections() { return m_sections; }
		/// Get the HashNames of all loaded keys in an INI section.
		/// @param section The HashName of the section to inspect.
		/// @return The HashNames of every key loaded from the section.
		/// @note The keys returned may be either value or dictionary lookup keys.
		inline const std::set<HashName>& Keys(HashName section) { return m_keys[section]; }
		/// Check whether a key exists in the loaded INI file.
		/// @param section The HashName of the section containing the key.
		/// @param key The HashName of the key to inspect.
		/// @return @c true if the key exists in the file.
		inline bool KeyExists(HashName section, HashName key) { return (m_keys[section].count(key)); }

		/// Get a value associated with a key.
		/// @param section The HashName of the section containing the value.
		/// @param key The HashName of the key to load.
		/// @param type The data type of the value.
		/// @return The value as an inivalue_t.
		inivalue_t GetValue(HashName section, HashName key, INIDataType type);
		/// Get all values associated with a key.
		/// @param section The HashName of the section containing the value.
		/// @param key The HashName of the key to load.
		/// @param type The data type of the values.
		/// @return The values as inivalue_t's.
		std::vector<inivalue_t> GetMultiValue(HashName section, HashName key, INIDataType type);

		/// Get a dictionary value associated with a key.
		/// @param section The HashName of the section containing the dictionary.
		/// @param key The HashName of the key associated with the dictionary.
		/// @param subkeys The HashNames and data types of the subkeys and subvalues to load.
		/// @return A map of the dictionary's subkeys and subvalues.
		std::map<HashName, inivalue_t> GetDictionary(HashName section, HashName key,
			std::initializer_list<std::pair<HashName, INIDataType>> subkeys);
		/// Get a dictionary value associated with a key.
		/// @param section The HashName of the section containing the dictionary.
		/// @param key The HashName of the key associated with the dictionary.
		/// @param subkeys The HashNames and data types of the subkeys and subvalues to load.
		/// @return A map of the dictionary's subkeys and subvalues.
		std::map<HashName, inivalue_t> GetDictionary(HashName section, HashName key,
			std::map<HashName, INIDataType> subkeys);
		/// Get a dictionary value associated with a key.
		/// @param section The HashName of the section containing the dictionary.
		/// @param key The HashName of the key associated with the dictionary.
		/// @param subkeys The HashNames and data types of the subkeys and subvalues to load.
		/// @return A map of the dictionary's subkeys and subvalues.
		std::map<HashName, inivalue_t> GetDictionary(HashName section, HashName key,
			std::unordered_map<HashName, INIDataType> subkeys);

		/// Get all dictionary values associated with a key.
		/// @param section The HashName of the section containing the dictionaries.
		/// @param key The HashName of the key associated with the dictionaries.
		/// @param subkeys The HashNames and data types of the subkeys and subvalues to load.
		/// @return All subkey/subvalue maps associated with the given key.
		std::vector<std::map<HashName, inivalue_t>> GetMultiDictionary(HashName section, HashName key,
			std::initializer_list<std::pair<HashName, INIDataType>> subkeys);
		/// Get all dictionary values associated with a key.
		/// @param section The HashName of the section containing the dictionaries.
		/// @param key The HashName of the key associated with the dictionaries.
		/// @param subkeys The HashNames and data types of the subkeys and subvalues to load.
		/// @return All subkey/subvalue maps associated with the given key.
		std::vector<std::map<HashName, inivalue_t>> GetMultiDictionary(HashName section, HashName key,
			std::map<HashName, INIDataType> subkeys);
		/// Get all dictionary values associated with a key.
		/// @param section The HashName of the section containing the dictionaries.
		/// @param key The HashName of the key associated with the dictionaries.
		/// @param subkeys The HashNames and data types of the subkeys and subvalues to load.
		/// @return All subkey/subvalue maps associated with the given key.
		std::vector<std::map<HashName, inivalue_t>> GetMultiDictionary(HashName section, HashName key,
			std::unordered_map<HashName, INIDataType> subkeys);

	private:
		std::string m_nativepath;
		std::set<HashName> m_sections;
		std::map<HashName, std::set<HashName>> m_keys;
		std::map<HashName, std::map<HashName, std::vector<std::string>>> m_data;
		std::map<HashName, std::map<HashName, std::vector<std::map<HashName, std::string>>>> m_dictdata;
		ErrorCode m_errorcode = ErrorCode::Null;
	};

	/// A helper class for writing data to INI files.
	/// To use it, create a class instance, open a file with Open() and use class methods to write formatted data.
	class INIWriter
	{
	public:
		INIWriter() {};
		/// Optionally open an INI file during object construction.
		/// @param path The virtual path of the INI file.
		/// @param mergeSections If @c true, unmodified sections in the file at @c path are merged into the new file.
		/// @param mergeKeys If @c true, unmodified keys in the file at @c path are merged into the new file.
		INIWriter(const std::string& path, bool mergeSections = false, bool mergeKeys = false)
		{
			Open(path, mergeSections, mergeKeys);
		}
		~INIWriter() { Close(); }

		/// Open an INI file for writing.
		/// @param path The virtual path of the INI file.
		/// @return The result of the file open operation.
		/// @param mergeSections If @c true, unmodified sections in the file at @c path are merged into the new file.
		/// @param mergeKeys If @c true, unmodified keys in the file at @c path are merged into the new file.
		void Open(const std::string& path, bool mergeSections = false, bool mergeKeys = false);
		/// Close the currently open INI file.
		/// @note This method is invoked automatically by the class destructor.
		/// @note It is safe to invoke this method even when no file is open.
		void Close();

		/// Write a comment to the INI file.
		/// @param comment The body of the comment.
		inline void Comment(const std::string& comment) { m_ss << "; " << comment << '\n'; }
		/// Write one or more newline characters to the INI file.
		/// @param count The number of newlines to write.
		inline void Newline(int count = 1) { for (int i = 0; i < count; i++) { m_ss << '\n'; } }
		/// Write a section header into the INI file.
		/// @param name The name of the new section.
		/// @note You can safely return to a section multiple times while writing to an INI file.
		void Section(const std::string& name);

		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, char value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, short value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, unsigned short value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, int value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, unsigned int value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, long value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, unsigned long value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, long long value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, unsigned long long value);

		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, float value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, double value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, long double value);

		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, bool value);

		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, std::string value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, const char* value);

		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, const void* value);
		/// Write a key/value pair to the INI file.
		/// @param key The key to associate with the value.
		/// @param value The data to serialize.
		void Value(const std::string& key, std::nullptr_t value);

		/// Write a dictionary value to the INI file.
		/// @param key The key to associate with the dictionary.
		/// @param dictionary The subkey/subvalue pairs to write to the dictionary.
		void Dictionary(const std::string& key, std::initializer_list<std::pair<std::string, inivalue_t>> dictionary);
		/// Write a dictionary value to the INI file.
		/// @param key The key to associate with the dictionary.
		/// @param dictionary The subkey/subvalue pairs to write to the dictionary.
		void Dictionary(const std::string& key, std::map<std::string, inivalue_t> dictionary);
		/// Write a dictionary value to the INI file.
		/// @param key The key to associate with the dictionary.
		/// @param dictionary The subkey/subvalue pairs to write to the dictionary.
		void Dictionary(const std::string& key, std::unordered_map<std::string, inivalue_t> dictionary);

	private:
		std::string m_nativepath;
		bool m_mergeSections = false;
		bool m_mergeKeys = false;

		bool m_isOpen = false;
		std::stringstream m_ss;
		HashName m_currentSection = HN_NULL;
		std::map<HashName, std::string> m_sectionHeaders;
		std::map<HashName, std::vector<std::pair<HashName, std::string>>> m_data;
	};


private:
	friend class Runtime;
	friend class Graphics;

#ifdef EP_BUILD_DYNAMIC
	friend bool ::Enterprise::LoadGameModule(const std::string& projectFilePath);
	friend void ::Enterprise::UnloadGameModule();
#endif

	static bool isAlphanumeric(const std::string& str);
	static bool isAlphanumeric(const std::string_view& str);

	static std::string GetNewTempFilename();

	static std::string contentDirPath;
	static std::string dataDirPath;
	static std::string saveDirPath;
	static std::string tempDirPath;
	static std::string shaderHeadersPath;

#ifdef EP_BUILD_DYNAMIC
	static std::string editorContentDirPath;
	static std::string editorDataDirPath;
	static std::string editorTempDirPath;

	// Sets editorContentDirPath, editorDataDirPath, editorTempDirPath, and shaderHeadersPath (unused with --sandbox)
	static void SetPlatformPathsForEditor();
#else
	// Sets contentDirPath, dataDirPath, saveDirPath, tempDirPath, and shaderHeadersPath (unused with --project)
	static void SetPlatformPathsForStandalone();
	// Sets shaderHeadersPath to editor install location (used with --project and without --sandbox)
	static void SetEditorPathForShaderHeaders();
#endif

	static void Init();
};

}

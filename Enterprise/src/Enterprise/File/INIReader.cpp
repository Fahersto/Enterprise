#include "EP_PCH.h"
#include "File.h"

using Enterprise::File;

// Helpers

static inline char strToChar(const std::string& str)
{
	char returnVal = '\0';

	size_t pos = str.find_first_not_of(" \t\r");
	if (pos != std::string::npos)
	{
		if (str[pos] == '\\' && pos + 1 < str.length())
		{
			pos++;
			switch (str[pos])
			{
			case '\\':
				returnVal = '\\';
				break;
			case '\'':
				returnVal = '\'';
				break;
			case '\"':
				returnVal = '\"';
				break;
			case 't':
				returnVal = '\t';
				break;
			case 'r':
				returnVal = '\r';
				break;
			case 'n':
				returnVal = '\n';
				break;
			default:
				EP_WARN("INIReader: Unhandled escape sequence \"\\{}\" detected in char \"{}\".",
					str[pos], str);
				returnVal = str[pos];
				break;
			}
		}
		else
		{
			returnVal = str[pos];
		}

		pos = str.find_first_not_of(" \t\r", pos + 1);
		if (pos != std::string::npos)
		{
			EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
		}
	}
	else
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'char'!", str);
	}

	return returnVal;
}

static inline short strToShort(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	long returnVal = std::strtol(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'short'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == LONG_MAX || returnVal == LONG_MIN) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'short'!");
	}
	else if (returnVal > SHRT_MAX || returnVal < SHRT_MIN)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'short'!");
	}

	return (short)returnVal;
}

static inline unsigned short strToUShort(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	unsigned long returnVal = std::strtoul(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'unsigned short'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == ULONG_MAX || returnVal == 0) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'unsigned short'!");
	}
	else if (returnVal > USHRT_MAX)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'unsigned short'!");
	}

	return (unsigned short)returnVal;
}

static inline int strToInt(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	long returnVal = std::strtol(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'int'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == LONG_MAX || returnVal == LONG_MIN) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'int'!");
	}
	else if (returnVal > INT_MAX || returnVal < INT_MIN)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'int'!");
	}

	return (int)returnVal;
}

static inline unsigned int strToUInt(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	unsigned long returnVal = std::strtoul(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'unsigned int'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == ULONG_MAX || returnVal == 0) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'unsigned int'!");
	}
	else if (returnVal > UINT_MAX)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'unsigned int'!");
	}

	return (unsigned int)returnVal;
}

static inline long strToLong(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	long returnVal = std::strtol(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'long'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == LONG_MAX || returnVal == LONG_MIN) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'long'!");
	}

	return returnVal;
}

static inline unsigned long strToULong(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	unsigned long returnVal = std::strtoul(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'unsigned long'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == ULONG_MAX || returnVal == 0) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'unsigned long'!");
	}

	return returnVal;
}

static inline long long strToLLong(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	long long returnVal = std::strtoll(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'long long'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == LLONG_MAX || returnVal == LLONG_MIN) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'long long'!");
	}

	return returnVal;
}

static inline unsigned long long strToULLong(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	unsigned long long returnVal = std::strtoull(str.c_str(), &firstUninterpretedChar, 0);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'unsigned long long'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if ((returnVal == ULLONG_MAX || returnVal == 0) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'unsigned long long'!");
	}

	return returnVal;
}


static inline float strToFloat(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	float returnVal = strtof(str.c_str(), &firstUninterpretedChar);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'float'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if (returnVal == HUGE_VALF && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'float'!");
	}

	return returnVal;
}

static inline double strToDouble(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	double returnVal = strtod(str.c_str(), &firstUninterpretedChar);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'double'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if (returnVal == HUGE_VAL && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'double'!");
	}

	return returnVal;
}

static inline long double strToLDouble(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	long double returnVal = strtold(str.c_str(), &firstUninterpretedChar);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'long double'!", str);
		returnVal = 0;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
	}
	else if (returnVal == HUGE_VALL && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'long double'!");
	}

	return returnVal;
}


static inline bool strToBool(const std::string& str)
{
	bool mightBeTrue = true, mightBeFalse = true;
	size_t pos = str.find_first_not_of(" \t\r");

	if (pos != std::string::npos)
	{
		if (str.length() >= pos + 4)
		{
			for (unsigned int i = 0; i < 4; i++)
			{
				if (str[i + pos] != "true"[i] && str[i + pos] != "TRUE"[i])
					mightBeTrue = false;
				if (str[i + pos] != "fals"[i] && str[i + pos] != "FALS"[i])
					mightBeFalse = false;
			}

			if (mightBeFalse && str.length() >= pos + 5)
			{
				if (str[pos + 4] != 'e' && str[pos + 4] != 'E')
					mightBeFalse = false;
			}

			if (!mightBeFalse && !mightBeTrue)
			{
				EP_WARN("INIReader: \"{}\" is not convertable to type 'bool'!", str);
			}
			else if (str.find_first_not_of(" \t\r", pos + 5) != std::string::npos)
			{
				EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
			}

			return mightBeTrue;
		}
		else
		{
			EP_WARN("INIReader: \"{}\" is not convertable to type 'bool'!", str);
		}
	}
	else
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'bool'!", str);
	}

	return false;
}

static inline std::string strSansEscapeChars(const std::string& str)
{
	std::string outStr(str);
	outStr.append(1, ' '); // prevents exception when reading past an escape character

	size_t i = 0;		// i: the current write index
	size_t skip = 0;	// i + skip: The current read index
	for (; i + skip < str.length(); i++)
	{
		if (outStr[i + skip] == '\\')
		{
			skip++;
			if (outStr[i + skip] == 't')
			{
				outStr[i] = 0x09;
				i++;
			}
			else if (outStr[i + skip] == 'r')
			{
				outStr[i] = 0x0d;
				i++;
			}
			else if (outStr[i + skip] == 'n')
			{
				outStr[i] = 0x0a;
				i++;
			}
			else if (outStr[i + skip] != '"' && outStr[i + skip] != '\'' && outStr[i + skip] != '\\')
			{
				EP_WARN("INIReader: Unhandled escape sequence \"\\{}\" detected in string \"{}\".", 
					outStr[i + skip], str);
			}
		}
		outStr[i] = outStr[i + skip];
	}

	outStr.resize(i);
	return outStr;
}

static inline void* strToPtr(const std::string& str)
{
	char* firstUninterpretedChar = nullptr;
	errno = 0;
	uint64_t returnVal = std::strtoull(str.c_str(), &firstUninterpretedChar, 16);

	if (firstUninterpretedChar == str.c_str())
	{
		EP_WARN("INIReader: \"{}\" is not convertable to type 'void*'!", str);
		return nullptr;
	}
	else if (*firstUninterpretedChar != '\0')
	{
		EP_WARN("INIReader: \"{}\" contains unexpected characters!", str);
		return nullptr;
	}
	else if ((returnVal == ULLONG_MAX || returnVal == 0) && errno == ERANGE)
	{
		EP_WARN("INIReader: \"{}\" is out of range for type 'void*'!");
		return nullptr;
	}

	return (void*)returnVal;
}


static inivalue_t strToINIData(const std::string& str, INIDataType type)
{
	switch (type)
	{
	case INIDataType::Short:	return strToShort(str);			break;
	case INIDataType::UShort:	return strToUShort(str);		break;
	case INIDataType::Int:		return strToInt(str);			break;
	case INIDataType::UInt:		return strToUInt(str);			break;
	case INIDataType::Long:		return strToLong(str);			break;
	case INIDataType::ULong:	return strToULong(str);			break;
	case INIDataType::LongLong: return strToLLong(str);			break;
	case INIDataType::ULongLong: return strToULLong(str);		break;
	case INIDataType::Float:	return strToFloat(str);			break;
	case INIDataType::Double:	return strToDouble(str);		break;
	case INIDataType::LDouble:	return strToLDouble(str);		break;
	case INIDataType::Bool:		return strToBool(str);			break;
	case INIDataType::Ptr:		return strToPtr(str);			break;
	case INIDataType::String:	return strSansEscapeChars(str); break;
	case INIDataType::Char:		return strToChar(str);			break;
	default: EP_ASSERT_NOENTRY(); break;
	}
}


// INIReader stuff

File::ErrorCode File::INIReader::Load(const std::string& path, const std::string& fallbackPath,
	std::initializer_list<const std::string> sectionFilters)
{
	m_nativepath = VirtualPathToNative(path);
	m_sections.clear();
	m_keys.clear();
	m_data.clear();
	m_dictdata.clear();

	std::vector<std::pair<std::string, bool>> validFilters; // First = Filter name, Second = IsGroupName
	bool inFilteredSection = sectionFilters.size() == 0;
	for (const std::string& filter : sectionFilters)
	{
		if (filter.length() == 0)
		{
			inFilteredSection = true;
			validFilters.push_back(std::pair(filter, false));
		}
		else if (isAlphanumeric(filter))
		{
			validFilters.push_back(std::pair(filter, filter.back() == '.'));
		}
	}

	TextFileReader ini;
	m_errorcode = ini.Open(path);
	bool readingFallbackFile = false;

	if (m_errorcode != ErrorCode::Success)
	{
		if (Exists(fallbackPath))
		{
			std::error_code ec;
			std::filesystem::copy(VirtualPathToNative(fallbackPath), m_nativepath, ec);
			if (ec)
			{
				EP_ERROR("INIReader::Load(): Error copying fallback INI!  File: \"{}\", Error: {}, {}", 
					VirtualPathToNative(fallbackPath), ec.value(), ec.message());
			}

			m_errorcode = ini.Open(fallbackPath);
			readingFallbackFile = true;
			if (m_errorcode != ErrorCode::Success)
			{
				EP_ERROR("INIReader::Load(): Cannot open fallback INI!  File: \"{}\", Error: {}",
					VirtualPathToNative(fallbackPath), ErrorCodeToStr(m_errorcode));
				return m_errorcode;
			}
		}
		else
		{
			EP_ERROR("INIReader::Load(): Could not open INI file \"{}\" or fallback \"{}\".", 
				m_nativepath, VirtualPathToNative(fallbackPath));
			return ErrorCode::DoesNotExist;
		}
	}


	std::string line;
	size_t pos, pos2, pos3;
	HashName section = HN_NULL;
	HashName key = HN_NULL;

	// Parse the file.
	while (!ini.isEOF())
	{
		line = ini.GetLine();
		pos = line.find_first_not_of(" \t\r");

		if (pos != std::string::npos)
		{
			if (line[pos] == '[')
				// This line may contain a section header.
			{
				pos2 = line.find_first_of(']', pos);
				if (pos2 != std::string::npos)
				{
					// Strip leading and trailing whitespace
					pos = line.find_first_not_of(" \t\r", pos + 1);

					bool garbageFollowsSectionHeader = false;
					pos3 = line.find_first_not_of(" \t\r", pos2 + 1);
					if (pos3 != std::string::npos)
					{
						if (line[pos3] != ';')
							garbageFollowsSectionHeader = true;
					}

					if (pos != pos2)
						pos2 = line.find_last_not_of(" \t\r", pos2 - 1) + 1;

					// Check if section passes filters
					if (validFilters.size() > 0)
					{
						for (const auto& [filterName, isFilter] : validFilters)
						{
							std::string_view sectionText(line.data() + pos, pos2 - pos);
							if (isFilter)
							{
								// Check for matching prefix
								if (sectionText.rfind(filterName, 0) == 0)
								{
									pos += filterName.length();
									section = HN(sectionText.data() + filterName.length(), sectionText.length() - filterName.length());
									m_sections.insert(section);
									inFilteredSection = true;
									break;
								}
								else
								{
									section = HN_NULL;
									inFilteredSection = false;
								}
							}
							else
							{
								// Look for exact match
								if (filterName == sectionText)
								{
									section = HN_NULL;
									inFilteredSection = true;
									m_sections.emplace(HN(filterName));
								}
								else
								{
									section = HN_NULL;
									inFilteredSection = false;
								}
							}
						}
					}
					else
					{
						section = HN(line.data() + pos, pos2 - pos);
						m_sections.insert(section);
					}

					if (garbageFollowsSectionHeader)
						EP_WARN("INIReader::Load(): Unexpected characters following section header \"[{}\".  "
							"File: \"{}\", Line: {}", HN_ToStr(section),
							(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
				}
				else
				{
					EP_WARN("INIReader::Load(): Invalid line encountered in INI file \"{}\".  Line was skipped.  Line:{}",
						(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
				}
			}
			else if (line[pos] != ';' && inFilteredSection)
				// This line may contain a key/value pair.
			{
				pos2 = line.find_first_of(" \t\r=;", pos);

				if (pos2 == std::string::npos)
				{
					// Delimiter is missing.
					EP_WARN("INIReader::Load(): Invalid line encountered in \"{}\" at line {}.  Line was skipped.",
						(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
				}
				else if (line[pos2] == ';')
				{
					// Comment precedes delimiter, if one exists
					EP_WARN("INIReader::Load(): Invalid line encountered in \"{}\" at line {}.  Line was skipped.",
						(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
				}
				else
				{
					std::string_view key_sv(line.data() + pos, pos2 - pos);
					if (!isAlphanumeric(key_sv))
					{
						EP_WARN("INIReader::Load(): Key \"{}\" contains special characters.  Key may not read "
							"correctly in external INI tools.  File: \"{}\", Line: {}", key_sv,
							(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
					}
					key = HN(line.data() + pos, pos2 - pos);

					// The next character may be the delimiter("=")
					pos = line.find_first_not_of(" \t\r", pos2);

					if (pos == std::string::npos)
					{
						// Delimiter is missing.
						EP_WARN("INIReader::Load(): Invalid line encountered in \"{}\" at line {}.  Line was skipped.",
							(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
					}
					else if (line[pos] == '=')
					{
						// We've got the delimiter.  Looking for the value now
						pos = line.find_first_not_of(" \t\r", pos + 1);

						if (pos == std::string::npos)
						{
							// The delimiter was the last character on the line.
							// This may have been intentional, so we add an empty string to m_data.
							m_data[section][key].emplace_back();
							m_keys[section].emplace(key);
						}
						else if (line[pos] == ';')
						{
							// A semicolon immediately follows the delimiter.
							// This may have been intentional, so we add an empty string to m_data.
							m_data[section][key].emplace_back();
							m_keys[section].emplace(key);
						}
						else if (line[pos] == '"' || line[pos] == '\'')
							// Value is quoted: spaces and semicolons are allowed.
						{
							// Look for the next quote that isn't escaped with '\'
							{
								bool inEscape = false;
								for (pos2 = pos + 1; pos2 < line.length(); pos2++)
								{
									if (inEscape)
									{
										inEscape = false;
									}
									else if (line[pos2] == line[pos])
									{
										break;
									}
									else if (line[pos2] == '\\')
									{
										inEscape = true;
									}
								}
							}

							if (pos2 >= line.length())
								// Unpaired quote
							{
								EP_WARN("INIReader::Load(): Value for key \"{}\" on line {} of \"{}\" is missing "
									"closing quote.  Line was skipped.", HN_ToStr(key), ini.LineNo(),
									(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath));
							}
							else
								// Found the closing quote.
							{
								m_data[section][key].emplace_back(line.substr(pos + 1, pos2 - pos - 1));
								m_keys[section].emplace(key);

								// Warn about any unexpected characters following quotes
								pos3 = line.find_first_not_of(" \t\r", pos2 + 1);
								if (pos3 != std::string::npos)
								{
									if (line[pos3] != ';')
									{
										EP_WARN("INIReader::Load(): Unexpected characters following quoted value.  "
											"Invalid characters ignored.  Key: \"{}/{}\", File: {}, Line: {}", HN_ToStr(key),
											(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
									}
								}
							}
						}
						else if (line[pos] == '(')
							// Dictionary value: catch and store key/value pairs in m_dictdata.
						{
							// Confirm value has closing ')'
							{
								bool inQuotes = false;
								char currentQuoteChar = 0x0;
								pos3 = pos + 1;
								for (; pos3 < line.length(); pos3++)
								{
									if (inQuotes)
									{
										if (line[pos3] == currentQuoteChar)
										{
											inQuotes = false;
										}
									}
									else
									{
										if (line[pos3] == ';' || line[pos3] == ')')
										{
											break;
										}
										else if (line[pos3] == '\'' || line[pos3] == '"')
										{
											inQuotes = true;
											currentQuoteChar = line[pos3];
										}
									}
								}
							}

							if (pos3 != std::string::npos)
							{
								if (line[pos3] == ';')
								{
									// Unpaired parenthesis
									EP_WARN("INIReader::Load(): Value for key \"{}\" on line {} of \"{}\" is missing "
										"closing parenthesis.  Line was skipped.", HN_ToStr(key), ini.LineNo(),
										(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath));
								}
								else // can only be ')'
								{
									m_dictdata[section][key].emplace_back();
									m_keys[section].emplace(key);

									// Thanks to the above check, we know in advance that there will be no comments or
									// unclosed quotes until the next ')'.

									HashName subkey = HN_NULL;
									for (pos++; pos < pos3; pos++)
									{
										// Look for next subkey
										pos = line.find_first_not_of(" \t\r", pos);
										if (line[pos] == '\'' || line[pos] == '"')
										{
											EP_WARN("INIReader::Load(): Encountered '{}' when parsing dictionary value "
												"for key.  Remainder of dictionary will be ignored.  File: \"{}\", Line: {}",
												line[pos],
												(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath),
												ini.LineNo());
											break;
										}
										else if (line[pos] == ')')
										{
											// There isn't a next key
											break;
										}
										else
										{
											pos2 = line.find_first_of(" \t\r=)", pos);
											subkey = HN(line.data() + pos, pos2 - pos);
										}

										// Look for '='
										pos = line.find_first_not_of(" \t\r", pos2);

										if (line[pos] == '=')
										{
											// Found it!
											// Leave this block empty
										}
										else if (pos == pos3) // line[pos] == ')'
										{
											EP_WARN("INIReader::Load(): Encountered unpaired subkey \"{}\" in "
												"dictionary value \"{}/{}\".  File: \"{}\", Line: {}",
												HN_ToStr(subkey), HN_ToStr(section), HN_ToStr(key),
												(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath),
												ini.LineNo());
											break;
										}
										else
										{
											EP_WARN("INIReader::Load(): Unexpected characters following subkey \"{}\" "
												"in dictionary value \"{}/{}\".  Remainder of dictionary will be "
												"ignored.  File: {}, Line: {}", HN_ToStr(subkey), HN_ToStr(section), HN_ToStr(key),
												(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath),
												ini.LineNo());
											break;
										}

										// Look for value
										pos = line.find_first_not_of(" \t\r", pos + 1);
										if (pos < pos3) // line[pos] != ')'
										{
											if (line[pos] == '\"' || line[pos] == '"')
												// value is in quotes
											{
												pos2 = line.find_first_of(line[pos], pos + 1);

												// Quotes are guaranteed to close prior to pos3: no need to check for npos
												m_dictdata[section][key].back().emplace(std::make_pair(subkey, line.substr(pos + 1, pos2 - pos - 1)));
												pos2++;
											}
											else
												// value is not quoted
											{
												pos2 = line.find_first_of(" \t\r,)", pos);
												m_dictdata[section][key].back().emplace(std::make_pair(subkey, line.substr(pos, pos2 - pos)));
											}
										}
										else
										{
											EP_WARN("INIReader::Load(): Encountered unpaired subkey \"{}\" in "
												"dictionary value \"{}/{}\".  File: \"{}\", Line: {}",
												HN_ToStr(subkey), HN_ToStr(section), HN_ToStr(key),
												(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath),
												ini.LineNo());
											break;
										}

										pos = line.find_first_of(",)", pos2);
										// pos will increment past ',' prior to next loop
									}
								}
							}
							else
								// Unpaired parenthesis
							{
								EP_WARN("INIReader::Load(): Value for key \"{}\" on line {} of \"{}\" is missing "
									"closing parenthesis.  Line was skipped.", HN_ToStr(key), ini.LineNo(),
									(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath));
							}
						}
						else
							// Standard value: find first whitespace or comment, then store value
						{
							pos2 = line.find_first_of(" \t\r;", pos);
							if (pos2 == std::string::npos)
							{
								m_data[section][key].emplace_back(line.substr(pos, line.length() - pos));
								m_keys[section].emplace(key);
							}
							else
							{
								m_data[section][key].emplace_back(line.substr(pos, pos2 - pos));
								m_keys[section].emplace(key);

								// Warn against extra characters following value
								if (line[pos2] != ';')
								{
									pos2 = line.find_first_not_of(" \t\r", pos2 + 1);
									if (pos2 != std::string::npos)
									{
										if (line[pos2] != ';')
										{
											// Non-comment characters following value.
											EP_WARN("INIReader::Load(): Unexpected characters following unquoted value.  "
												"Invalid characters ignored.  Key: \"{}/{}\", File: {}, Line: {}",
												HN_ToStr(key),
												(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath),
												ini.LineNo());
										}
									}
								}
							}
						}
					}
					else
					{
						// Delimiter is missing.
						EP_WARN("INIReader::Load(): Invalid line encountered in \"{}\" at line {}.  Line was skipped.",
							(readingFallbackFile ? VirtualPathToNative(fallbackPath) : m_nativepath), ini.LineNo());
					}
				}
			}
		}
	}

	return m_errorcode;
}


inivalue_t File::INIReader::GetValue(HashName section, HashName key, INIDataType type)
{
	if (m_data.count(section) != 0)
	{
		if (m_data[section].count(key) != 0)
		{
			if (m_data[section][key].size() > 1)
			{
				EP_WARN("INIReader::GetValue(): File contains multiple \"{}/{}\" keys.  "
					"Use GetMultiShort() to read all values.", HN_ToStr(section), HN_ToStr(key));
			}

			return strToINIData(m_data[section][key].front(), type);
		}
		else
		{
			EP_WARN("INIReader::GetValue(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetValue(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return 0;
}

std::vector<inivalue_t> File::INIReader::GetMultiValue(HashName section, HashName key, INIDataType type)
{
	std::vector<inivalue_t> returnVal;

	if (m_data.count(section) != 0)
	{
		if (m_data[section].count(key) != 0)
		{
			for (const std::string& str : m_data[section][key])
			{
				returnVal.push_back(strToINIData(str, type));
			}
		}
		else
		{
			EP_WARN("INIReader::GetMultiValue(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetMultiValue(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}


std::map<HashName, inivalue_t> File::INIReader::GetDictionary(HashName section, HashName key, 
	std::initializer_list<std::pair<HashName, INIDataType>> subkeys)
{
	std::map<HashName, inivalue_t> returnVal;

	// Undupe matching subkeys in subkeys
	std::map<HashName, INIDataType> undupedSubkeys;
	for (const auto& [subkey, subvalue] : subkeys)
	{
		if (undupedSubkeys.count(subkey) != 0)
		{
			EP_WARN("INIReader::GetDictionary(): More than one instance of the subkey \"{}\" was provided in 'subkeys'.  ",
				"Key: \"{}/{}\"", HN_ToStr(subkey), HN_ToStr(section), HN_ToStr(key));
		}
		else
		{
			undupedSubkeys[subkey] = subvalue;
		}
	}

	if (m_dictdata.count(section) != 0)
	{
		if (m_dictdata[section].count(key) != 0)
		{
			if (m_dictdata[section][key].size() > 1)
			{
				EP_WARN("INIReader::GetDictionary(): File contains multiple \"{}/{}\" keys.  "
					"Use GetMultiDictionary() to read all values.", HN_ToStr(section), HN_ToStr(key));
			}

			for (const auto& [subkey, subvalue] : m_dictdata[section][key].front())
			{
				if (undupedSubkeys.count(subkey) != 0)
				{
					returnVal[subkey] = strToINIData(subvalue, undupedSubkeys[subkey]);
				}
			}
		}
		else
		{
			EP_WARN("INIReader::GetDictionary(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetDictionary(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}

std::map<HashName, inivalue_t> File::INIReader::GetDictionary(HashName section, HashName key, 
	std::map<HashName, INIDataType> subkeys)
{
	std::map<HashName, inivalue_t> returnVal;

	if (m_dictdata.count(section) != 0)
	{
		if (m_dictdata[section].count(key) != 0)
		{
			if (m_dictdata[section][key].size() > 1)
			{
				EP_WARN("INIReader::GetDictionary(): File contains multiple \"{}/{}\" keys.  "
					"Use GetMultiDictionary() to read all values.", HN_ToStr(section), HN_ToStr(key));
			}

			for (const auto& [subkey, subvalue] : m_dictdata[section][key].front())
			{
				if (subkeys.count(subkey) != 0)
				{
					returnVal[subkey] = strToINIData(subvalue, subkeys[subkey]);
				}
			}
		}
		else
		{
			EP_WARN("INIReader::GetDictionary(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetDictionary(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}

std::map<HashName, inivalue_t> File::INIReader::GetDictionary(HashName section, HashName key,
	std::unordered_map<HashName, INIDataType> subkeys)
{
	std::map<HashName, inivalue_t> returnVal;

	if (m_dictdata.count(section) != 0)
	{
		if (m_dictdata[section].count(key) != 0)
		{
			if (m_dictdata[section][key].size() > 1)
			{
				EP_WARN("INIReader::GetDictionary(): File contains multiple \"{}/{}\" keys.  "
					"Use GetMultiDictionary() to read all values.", HN_ToStr(section), HN_ToStr(key));
			}

			for (const auto& [subkey, subvalue] : m_dictdata[section][key].front())
			{
				if (subkeys.count(subkey) != 0)
				{
					returnVal[subkey] = strToINIData(subvalue, subkeys[subkey]);
				}
			}
		}
		else
		{
			EP_WARN("INIReader::GetDictionary(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetDictionary(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}


std::vector<std::map<HashName, inivalue_t>> File::INIReader::GetMultiDictionary(HashName section, HashName key, 
	std::initializer_list<std::pair<HashName, INIDataType>> subkeys)
{
	std::vector<std::map<HashName, inivalue_t>> returnVal;

	// Undupe matching subkeys in subkeys
	std::map<HashName, INIDataType> undupedSubkeys;
	for (const auto& [subkey, subvalue] : subkeys)
	{
		if (undupedSubkeys.count(subkey) != 0)
		{
			EP_WARN("INIReader::GetDictionary(): More than one instance of the subkey \"{}\" was provided in 'subkeys'.  ",
				"Key: \"{}/{}\"", HN_ToStr(subkey), HN_ToStr(section), HN_ToStr(key));
		}
		else
		{
			undupedSubkeys[subkey] = subvalue;
		}
	}

	if (m_dictdata.count(section) != 0)
	{
		if (m_dictdata[section].count(key) != 0)
		{
			for (const std::map<HashName, std::string>& dict: m_dictdata[section][key])
			{
				returnVal.emplace_back();

				for (const auto& [subkey, subvalue] : dict)
				{
					if (undupedSubkeys.count(subkey) != 0)
					{
						returnVal.back()[subkey] = strToINIData(subvalue, undupedSubkeys[subkey]);
					}
				}
			}
		}
		else
		{
			EP_WARN("INIReader::GetDictionary(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetDictionary(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}

std::vector<std::map<HashName, inivalue_t>> File::INIReader::GetMultiDictionary(HashName section, HashName key,
	std::map<HashName, INIDataType> subkeys)
{
	std::vector<std::map<HashName, inivalue_t>> returnVal;

	if (m_dictdata.count(section) != 0)
	{
		if (m_dictdata[section].count(key) != 0)
		{
			for (const std::map<HashName, std::string>& dict: m_dictdata[section][key])
			{
				returnVal.emplace_back();

				for (const auto& [subkey, subvalue] : dict)
				{
					if (subkeys.count(subkey) != 0)
					{
						returnVal.back()[subkey] = strToINIData(subvalue, subkeys[subkey]);
					}
				}
			}
		}
		else
		{
			EP_WARN("INIReader::GetDictionary(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetDictionary(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}

std::vector<std::map<HashName, inivalue_t>> File::INIReader::GetMultiDictionary(HashName section, HashName key,
	std::unordered_map<HashName, INIDataType> subkeys)
{
	std::vector<std::map<HashName, inivalue_t>> returnVal;

	if (m_dictdata.count(section) != 0)
	{
		if (m_dictdata[section].count(key) != 0)
		{
			for (const std::map<HashName, std::string>& dict: m_dictdata[section][key])
			{
				returnVal.emplace_back();

				for (const auto& [subkey, subvalue] : dict)
				{
					if (subkeys.count(subkey) != 0)
					{
						returnVal.back()[subkey] = strToINIData(subvalue, subkeys[subkey]);
					}
				}
			}
		}
		else
		{
			EP_WARN("INIReader::GetDictionary(): Key \"{}/{}\" was not parsed from INI file.", HN_ToStr(section), HN_ToStr(key));
		}
	}
	else
	{
		EP_WARN("INIReader::GetDictionary(): Section \"{}\" was not parsed from INI file.", HN_ToStr(section));
	}

	return returnVal;
}

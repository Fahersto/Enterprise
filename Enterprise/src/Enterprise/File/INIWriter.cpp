#include "EP_PCH.h"
#include "File.h"

namespace Enterprise
{

void File::INIWriter::Open(const std::string& path, bool mergeSections, bool mergeKeys)
{
	m_nativepath = VirtualPathToNative(path);
	m_mergeSections = mergeSections;
	m_mergeKeys = mergeKeys;

	m_isOpen = true;
	m_currentSection = HN_NULL;
	m_ss.str(std::string());
	m_data.clear();
	m_sectionHeaders.clear();
}

void File::INIWriter::Close()
{
	if (m_isOpen)
	{
		m_isOpen = false;
		TextFileReader originalFileIn(m_nativepath);
		TextFileWriter mergedFileOut(m_nativepath);

		std::string line;
		size_t pos, pos2;
		std::stringstream commentBuffer;

		HashName currentSection = HN_NULL;
		std::set<HashName> previouslyParsedSections;
		previouslyParsedSections.emplace(HN_NULL);
		std::set<std::pair<HashName, HashName>> previouslyWrittenKeys;
		bool currentlyMergingKeys = m_data.count(HN_NULL) ? m_mergeKeys : m_mergeSections;

		// Write global section
		for (const auto& [key, text] : m_data[HN_NULL])
		{
			previouslyWrittenKeys.emplace(HN_NULL, key);
			mergedFileOut << text;
		}

		// Parse original file for merge
		while (!originalFileIn.isEOF())
		{
			line = originalFileIn.GetLine();
			pos = line.find_first_not_of(" \t\r");

			if (pos != std::string::npos)
			{
				if (line[pos] == '[')
					// This line may contain a section.
				{
					pos2 = line.find_first_of(']', pos);
					if (pos2 != std::string::npos)
					{
						// Get the section name
						pos = line.find_first_not_of(" \t\r", pos + 1);
						if (pos != pos2)
							pos2 = line.find_last_not_of(" \t\r", pos2 - 1) + 1;
						currentSection = HN(line.data() + pos, pos2 - pos);

						if (m_data.count(currentSection))
							// There is new data to write to this section.
						{
							// header
							mergedFileOut << m_sectionHeaders[currentSection];

							// data
							if (previouslyParsedSections.emplace(currentSection).second)
							{
								for (const auto& [key, text] : m_data[currentSection])
								{
									previouslyWrittenKeys.emplace(std::pair(currentSection, key));
									mergedFileOut << text;
								}
							}

							currentlyMergingKeys = m_mergeKeys;
						}
						else
						{
							if (m_mergeSections)
								// This section isn't modified, but we are merging it
							{
								// header
								mergedFileOut << commentBuffer.str();
								mergedFileOut << line << '\n';
							}
							currentlyMergingKeys = m_mergeSections;
						}
						commentBuffer.str(std::string());
					}
				}
				else if (line[pos] != ';')
				{
					// This line may contain a key.
					if (currentlyMergingKeys)
					{
						// Get the key name
						pos2 = line.find_first_of(" \t\r=;", pos);
						if (pos2 != std::string::npos)
						{
							HashName key = HN(line.data() + pos, pos2 - pos);

							// copy the key/value pair and any comments
							if (previouslyWrittenKeys.count(std::pair(currentSection, key)) == 0)
							{
								mergedFileOut << commentBuffer.str();
								mergedFileOut << line << '\n';
							}
						}
						else
							// line is badly formatted: but copy it anyways
							// warnings will appear on next file read
						{
							mergedFileOut << commentBuffer.str();
							mergedFileOut << line << '\n';
						}
					}

					commentBuffer.str(std::string());
				}
				else
				{
					commentBuffer << line << '\n';
				}
			}
			else
			{
				if (!originalFileIn.isEOF())
					commentBuffer << '\n';
			}
		}

		// Write out sections not already present in the file
		mergedFileOut << commentBuffer.str();
		for (const auto& [section, v] : m_data)
		{
			if (previouslyParsedSections.count(section) == 0)
			{
				mergedFileOut << m_sectionHeaders[section];

				for (const auto& [key, text] : m_data[section])
				{
					mergedFileOut << text;
				}
			}
		}

		originalFileIn.Close();
		mergedFileOut.Close();
	}
}


void File::INIWriter::Section(const std::string& name)
{
	if (m_isOpen)
	{
		if (!isAlphanumeric(name))
			EP_WARN("INIWriter::Section(): \"{}\" is not a valid section name.  "
				"It may present parsing issues upon reload.", name);

		m_ss << '[' << name << "]\n";

		m_currentSection = HN(name);
		m_data[m_currentSection];
		m_sectionHeaders[m_currentSection] = m_ss.str();
		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Section(): No file is open!");
	}
}


void File::INIWriter::Value(const std::string& key, char value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			switch (value)
			{
			case '\\':
				m_ss << key << " = " << "\'\\\\\'\n";
				break;
			case '\'':
				m_ss << key << " = " << "\'\\'\'\n";
				break;
			case '\"':
				m_ss << key << " = " << "\'\\\"\'\n";
				break;
			case '\t':
				m_ss << key << " = " << "\'\\t\'\n";
				break;
			case '\r':
				m_ss << key << " = " << "\'\\r\'\n";
				break;
			case '\n':
				m_ss << key << " = " << "\'\\n\'\n";
				break;
			default:
				m_ss << key << " = " << '\'' << value << "\'\n";
				break;
			}
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}

}

void File::INIWriter::Value(const std::string& key, short value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, unsigned short value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, int value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, unsigned int value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, long value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, unsigned long value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, long long value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, unsigned long long value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}


void File::INIWriter::Value(const std::string& key, float value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, double value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, long double value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}


void File::INIWriter::Value(const std::string& key, bool value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << (value ? "true" : "false") << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}


static std::string strWithEscapeChars(const std::string& str)
{
	std::string outStr;
	outStr.reserve(str.length());

	for (const char& c : str)
	{
		switch (c)
		{
		case '\\':
			outStr.append("\\\\");
			break;
		case '\'':
			outStr.append("\\'");
			break;
		case '\"':
			outStr.append("\\\"");
			break;
		case '\t':
			outStr.append("\\t");
			break;
		case '\r':
			outStr.append("\\r");
			break;
		case '\n':
			outStr.append("\\n");
			break;
		default:
			outStr.append(1, c);
			break;
		}
	}

	return outStr;
}


void File::INIWriter::Value(const std::string& key, std::string value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = '" << strWithEscapeChars(value) << "'\n";
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, const char* value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = '" << strWithEscapeChars(value) << "'\n";
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}


void File::INIWriter::Value(const std::string& key, const void* value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}

void File::INIWriter::Value(const std::string& key, std::nullptr_t value)
{
	if (m_isOpen)
	{
		if (isAlphanumeric(key))
		{
			m_ss << key << " = " << value << '\n';
			m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		}
		else
		{
			EP_WARN("INIWriter::Value(): \"{}\" is not a valid key.  Value will not be written.", key);
		}

		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Value(): No file is open!");
	}
}


void File::INIWriter::Dictionary(const std::string& key, std::initializer_list<std::pair<std::string, inivalue_t>> dictionary)
{
	if (m_isOpen)
	{
		if (!isAlphanumeric(key))
		{
			EP_WARN("INIWriter::Dictionary(): \"{}\" is not a valid key.  Dictionary will not be written.", key);
			m_ss.str(std::string());
			return;
		}

		m_ss << key << " = (";

		std::set<HashName> usedSubkeys;
		bool isFirstLoop = true;
		for (const auto& [subkey, subvalue] : dictionary)
		{
			if (!usedSubkeys.emplace(HN(subkey)).second)
			{
				EP_WARN("INIWriter::Dictionary(): The subkey \"{}\" is specified more than once.  Subvalue will not be written.", subkey);
				continue;
			}

			if (!isAlphanumeric(subkey))
			{
				EP_WARN("INIWriter::Dictionary(): \"{}\" is not a valid subkey.  Subvalue will not be written.", subkey);
				continue;
			}

			if (isFirstLoop)
				isFirstLoop = false;
			else
				m_ss << ", ";

			static_assert(std::is_same_v<std::variant_alternative_t<0, decltype(inivalue_t::value)>, char>);
			static_assert(std::is_same_v<std::variant_alternative_t<13, decltype(inivalue_t::value)>, std::string>);
			static_assert(std::is_same_v<std::variant_alternative_t<14, decltype(inivalue_t::value)>, const char*>);

			if (subvalue.value.index() == 0 || subvalue.value.index() == 13 || subvalue.value.index() == 14)
				m_ss << subkey << "='" << subvalue << '\'';
			else
				m_ss << subkey << "=" << subvalue;
		}

		m_ss << ")\n";
		m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Dictionary(): No file is open!");
	}
}

void File::INIWriter::Dictionary(const std::string& key, std::map<std::string, inivalue_t> dictionary)
{
	if (m_isOpen)
	{
		if (!isAlphanumeric(key))
		{
			EP_WARN("INIWriter::Dictionary(): \"{}\" is not a valid key.  Dictionary will not be written.", key);
			m_ss.str(std::string());
			return;
		}

		m_ss << key << " = (";

		bool isFirstLoop = true;
		for (const auto& [subkey, subvalue] : dictionary)
		{
			if (!isAlphanumeric(subkey))
			{
				EP_WARN("INIWriter::Dictionary(): \"{}\" is not a valid subkey.  Subvalue will not be written.", subkey);
				continue;
			}

			if (isFirstLoop)
				isFirstLoop = false;
			else
				m_ss << ", ";

			static_assert(std::is_same_v<std::variant_alternative_t<0, decltype(inivalue_t::value)>, char>);
			static_assert(std::is_same_v<std::variant_alternative_t<13, decltype(inivalue_t::value)>, std::string>);
			static_assert(std::is_same_v<std::variant_alternative_t<14, decltype(inivalue_t::value)>, const char*>);

			if (subvalue.value.index() == 0 || subvalue.value.index() == 13 || subvalue.value.index() == 14)
				m_ss << subkey << "='" << subvalue << '\'';
			else
				m_ss << subkey << "=" << subvalue;
		}

		m_ss << ")\n";
		m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Dictionary(): No file is open!");
	}
}

void File::INIWriter::Dictionary(const std::string& key, std::unordered_map<std::string, inivalue_t> dictionary)
{
	if (m_isOpen)
	{
		if (!isAlphanumeric(key))
		{
			EP_WARN("INIWriter::Dictionary(): \"{}\" is not a valid key.  Dictionary will not be written.", key);
			m_ss.str(std::string());
			return;
		}

		m_ss << key << " = (";

		bool isFirstLoop = true;
		for (const auto& [subkey, subvalue] : dictionary)
		{
			if (!isAlphanumeric(subkey))
			{
				EP_WARN("INIWriter::Dictionary(): \"{}\" is not a valid subkey.  Subvalue will not be written.", subkey);
				continue;
			}

			if (isFirstLoop)
				isFirstLoop = false;
			else
				m_ss << ", ";

			static_assert(std::is_same_v<std::variant_alternative_t<0, decltype(inivalue_t::value)>, char>);
			static_assert(std::is_same_v<std::variant_alternative_t<13, decltype(inivalue_t::value)>, std::string>);
			static_assert(std::is_same_v<std::variant_alternative_t<14, decltype(inivalue_t::value)>, const char*>);

			if (subvalue.value.index() == 0 || subvalue.value.index() == 13 || subvalue.value.index() == 14)
				m_ss << subkey << "='" << subvalue << '\'';
			else
				m_ss << subkey << "=" << subvalue;
		}

		m_ss << ")\n";
		m_data[m_currentSection].push_back(std::pair(HN(key), m_ss.str()));
		m_ss.str(std::string());
	}
	else
	{
		EP_WARN("INIWriter::Dictionary(): No file is open!");
	}
}

}

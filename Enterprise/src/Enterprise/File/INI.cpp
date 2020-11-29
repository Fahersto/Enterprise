#include "EP_PCH.h"
#include "File.h"

using Enterprise::File;


std::pair<bool, bool> File::INIStringToBool(const std::string& str)
{
	// Remove whitespace.
	size_t pos = str.find_first_not_of(' ');
	if (pos == std::string::npos) { pos = 0; }
	size_t pos2 = str.find_first_of(' ', pos);
	if (pos2 == std::string::npos) { pos2 = str.size(); }

	if (str.find_first_not_of(' ', pos2) == std::string::npos)
	{
		// Either there's no whitespace after the first term or there's only whitespace.
		std::string reducedStr = str.substr(pos, pos2 - pos);

		if (reducedStr == "true" || reducedStr == "True" || reducedStr == "TRUE" ||
			reducedStr == "yes" || reducedStr == "Yes" || reducedStr == "YES" ||
			reducedStr == "on" || reducedStr == "On" || reducedStr == "ON" ||
			reducedStr == "1")
		{
			return std::pair(true, true);
		}
		else if (reducedStr == "false" || reducedStr == "False" || reducedStr == "FALSE" ||
				 reducedStr == "no" || reducedStr == "No" || reducedStr == "NO" ||
				 reducedStr == "off" || reducedStr == "Off" || reducedStr == "OFF" ||
				 reducedStr == "0")
		{
			return std::pair(true, false);
		}
		else
		{
			if (reducedStr.empty())
			{
				EP_WARN("File System: Unable to convert empty INI value to type 'bool'.");
			}
			else
			{
				EP_WARN("File System: INI value string didn't parse correctly to type 'bool'."
						"  Value: \"{}\"", str);
			}
			return std::pair(false, false);
		}
	}
	else
	{
		// There's something besides the first term.
		EP_WARN("File System: INI value string didn't parse correctly to type 'bool'."
				"  Value: \"{}\"", str);
		return std::pair(false, false);
	}
}


std::pair<bool, int> File::INIStringToInt(const std::string& str)
{
	std::pair<bool, int> returnVal = { true, 0 };
	size_t pos = 0;

	try
	{
		returnVal.second = std::stoi(str, &pos);
		if (pos < str.length())
		{
			// confirm it's only whitespace after this.
			pos = str.find_first_not_of(' ', pos);
			if (pos != std::string::npos)
			{
				// Confirm this isn't a valid floating-point number.
				if (str.at(pos) == '.')
				{
					returnVal.second = (int)std::stof(str, &pos);
					pos = str.find_first_not_of(' ', pos);
					if (pos != std::string::npos)
					{
						// There's unexpected characters after the number.
						EP_WARN("File System: INI value string didn't parse correctly to type 'int'."
								"  Value: \"{}\"", str);
						returnVal.first = false;
					}
				}
				else
				{
					// There's unexpected characters after the number.
					EP_WARN("File System: INI value string didn't parse correctly to type 'int'."
							"  Value: \"{}\"", str);
					returnVal.first = false;
				}
			}
		}
	}
	catch (...)
	{
		if (str.empty())
		{
			EP_WARN("File System: Unable to convert empty INI value to type 'int'.");
		}
		else
		{
			EP_WARN("File System: INI value string didn't parse correctly to type 'int'."
					"  Value: \"{}\"", str);
		}
		returnVal.first = false;
	}

	return returnVal;
}


std::pair<bool, float> File::INIStringToFloat(const std::string& str)
{
	std::pair<bool, float> returnVal = { true, 0.0f };
	size_t pos = 0;

	try
	{
		returnVal.second = std::stof(str, &pos);
		if (pos < str.length())
		{
			// confirm it's only whitespace after this.
			pos = str.find_first_not_of(' ', pos);
			if (pos != std::string::npos)
			{
				// There's unexpected characters after the number.
				EP_WARN("File System: INI value string didn't parse correctly to type 'float'."
						"  Value: \"{}\"", str);
				returnVal.first = false;
			}
		}
	}
	catch (...)
	{
		if (str.empty())
		{
			EP_WARN("File System: Unable to convert empty INI value to type 'float'.");
		}
		else
		{
			EP_WARN("File System: INI value string didn't parse correctly to type 'float'."
					"  Value: \"{}\"", str);
		}
		returnVal.first = false;
	}

	return returnVal;
}


std::unordered_map<HashName, std::string> File::INIStringToDictionary(const std::string& str)
{
	std::unordered_map<HashName, std::string> returnVal;

	size_t pos = str.find_first_not_of(' ');
	if (pos != std::string::npos)
	{
		if (str.at(pos) == '(')
		{
			HashName subkey;
			size_t pos2;

			// Each iteration of this loop finds a "key=value," pair.
			while (pos != std::string::npos)
			{
				// Subkey -------------------------------------
				pos = str.find_first_not_of(' ', pos + 1);
				if (pos == std::string::npos)
				{
					// Line cuts off before expected subkey
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}
				else if (str.at(pos) == ';')
				{
					// Encountered comment before expected subkey.
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}

				pos2 = str.find_first_of(" =;", pos);
				if (pos2 == std::string::npos)
				{
					// No characters follow subkey on line
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}
				else if (str.at(pos2) == ';')
				{
					// The subkey was immediately followed by a comment.
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}

				subkey = HN(str.substr(pos, pos2 - pos));

				// '=' -------------------------------------
				pos = str.find_first_not_of(' ', pos2);
				if (pos == std::string::npos)
				{
					// Line ends bfore '='
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}
				else if (str.at(pos) != '=')
				{
					// Second identifier or ';' before '='
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}

				// Subvalue -------------------------------------
				pos = str.find_first_not_of(' ', pos + 1);
				if (pos == std::string::npos)
				{
					// Line ends after '='
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}
				else if (str.at(pos) == ';')
				{
					// Comment after '='.
					EP_WARN("File System: INI value could not be converted to type "
							"'std::unordered_map'.  Empty dictionary returned."
							"Value: \"{}\"", str);
					break;
				}
				else if (str.at(pos) == '\"')
				{
					// This is a string in quotes, so we tolerate spaces.
					pos2 = str.find_first_of('\"', pos + 1);
					if (pos2 == std::string::npos)
					{
						// Line ends before closing quote
						EP_WARN("File System: INI value could not be converted to type "
								"'std::unordered_map'.  Empty dictionary returned."
								"Value: \"{}\"", str);
						break;
					}

					returnVal[subkey] = str.substr(pos + 1, pos2 - pos - 1);

					// Look for comma or ')'.
					pos = str.find_first_not_of(' ', pos2 + 1);
					if (pos == std::string::npos)
					{
						// Line ends before ',' or ')'.
						EP_WARN("File System: INI value could not be converted to type "
								"'std::unordered_map'.  Empty dictionary returned."
								"Value: \"{}\"", str);
						break;
					}
					else if (str.at(pos) == ')')
					{
						// We've made it to the end.  Now, we must ensure that only whitespace and comments follow it.
						pos = str.find_first_not_of(' ', pos + 1);
						if (pos != std::string::npos)
						{
							if (str.at(pos) == ';')
							{
								// We're good!
								break;
							}
							else
							{
								// There are unexpected characters on the line following the dictionary value.
								// Technically, we can still use this dictionary, so post a warning, but still return the value.
								EP_WARN("File System: Unexpected characters following complete dictionary value.  "
										"Dictionary value still returned."
										"Value: \"{}\"", str);
								break;
							}
						}
						else
						{
							// We're good!
							break;
						}
					}
					else if (str.at(pos) == ',')
					{
						// There's another subkey.
						pos++;
						if (pos >= str.size())
						{
							// There's no more characters after the comma.
							EP_WARN("File System: INI value could not be converted to type "
									"'std::unordered_map'.  Empty dictionary returned."
									"Value: \"{}\"", str);
							break;
						}

						// Go for another key/value pair.
						continue;
					}
					else
					{
						// Subvalue not followed by ')' or ','.
						EP_WARN("File System: INI value could not be converted to type "
								"'std::unordered_map'.  Empty dictionary returned."
								"Value: \"{}\"", str);
						break;
					}
				}
				else
				{
					// Not a string in quotes: we do not tolerate spaces.
					pos2 = str.find_first_of(",) ;", pos);
					if (pos2 == std::string::npos)
					{
						// No characters follow the value.
						EP_WARN("File System: INI value could not be converted to type "
								"'std::unordered_map'.  Empty dictionary returned."
								"Value: \"{}\"", str);
						break;
					}
					else if (pos == pos2)
					{
						// The subvalue is missing.
						EP_WARN("File System: INI value could not be converted to type "
								"'std::unordered_map'.  Empty dictionary returned."
								"Value: \"{}\"", str);
						break;
					}

					returnVal[subkey] = str.substr(pos, pos2 - pos);

					pos = str.find_first_not_of(' ', pos2);
					if (str.at(pos) == ')')
					{
						// We're at the end: just gotta be sure it's all whitespace and comments from here.
						pos = str.find_first_not_of(' ', pos + 1);
						if (pos != std::string::npos)
						{
							if (str.at(pos) == ';')
							{
								// We're all good!
								break;
							}
							else
							{
								// There are unexpected characters on the line following the dictionary value.
								// Technically, we can still use this dictionary, so post a warning, but still return the value.
								EP_WARN("File System: Unexpected characters following valid dictionary value.  "
										"Dictionary value was returned."
										"Value: \"{}\"", str);
								break;
							}
						}
					}
					else if (str.at(pos) == ',')
					{
						// There's another subkey.
						pos++;
						if (pos >= str.size())
						{
							// There are no other characters after the comma.
							EP_WARN("File System: INI value could not be converted to type "
									"'std::unordered_map'.  Empty dictionary returned."
									"Value: \"{}\"", str);
							break;
						}

						// Go for another key/value pair.
						continue;
					}
					else
					{
						// Subvalue not followed by ')' or ','.
						EP_WARN("File System: INI value could not be converted to type "
								"'std::unordered_map'.  Empty dictionary returned."
								"Value: \"{}\"", str);
						break;
					}
				}
			}
		}
		else
		{
			// Invalid, doesn't start with '('
			EP_WARN("File System: INI value could not be converted to type 'std::unordered_map'.  "
					"Empty dictionary returned.  Value: \"{}\"", str);
		}
	}
	else
	{
		// Value string is empty
		EP_WARN("File System: Empty INI value could not be converted to type "
				"'std::unordered_map'.  Empty dictionary returned.");
	}

	return returnVal;
}


File::ErrorCode File::INIReader::Load(const std::string& path, bool areErrorsFatal, const std::string& sectionFilter)
{
	// TODO: Handle base INI reload mechanism
	// TODO: Handle Constants section

	bool filterIsAGroup;
	if (sectionFilter.empty())
	{
		filterIsAGroup = true;
	}
	else
	{
		EP_ASSERTF(sectionFilter.front() != ' ', "No leading whitespace is allowed in sectionFilter parameter");
		EP_ASSERTF(sectionFilter.back() != ' ', "No trailing whitespace is allowed in sectionFilter parameter");
		filterIsAGroup = sectionFilter.back() == '/';
	}

	m_path = path;
	TextFileReader ini(path);
	m_errorcode = ini.GetError();

	if (m_errorcode == ErrorCode::Success)
	{
		std::string line;
		size_t pos, pos2, pos3;
		HashName section = HN_NULL;
		HashName key;

		bool inQualifiedSection;
		if (sectionFilter.empty()) { inQualifiedSection = true; } // No filter
		else { inQualifiedSection = false; } // Using section filter, ignore sectionless keys
		
		while (!ini.isEOF())
		{
			line = ini.ReadNextLine();
			pos = line.find_first_not_of(' ');

			if (pos != std::string::npos) {
				if (line.at(pos) == '[')
				{
					// This may be a section.

					pos2 = line.find_first_of(']', pos);
					if (pos2 != std::string::npos)
					{
						// Strip leading and trailing whitespace
						pos = line.find_first_not_of(' ', pos + 1);
						pos2 = line.find_last_not_of(' ', pos2 - 1) + 1;

						// Qualify section based on filter
						if (filterIsAGroup)
						{
							// Check for matching prefix
							if (line.find(sectionFilter, pos) == pos)
							{
								pos += sectionFilter.size();
								section = HN(line.substr(pos, pos2 - pos));
								m_sections.push_back(section);
								inQualifiedSection = true;
							}
							else
							{
								section = HN_NULL;
								inQualifiedSection = false;
							}
						}
						else
						{
							// Look for exact match
							if (sectionFilter == line.substr(pos, pos2 - pos))
							{
								section = HN_NULL;
								inQualifiedSection = true;
							}
							else
							{
								section = HN_NULL;
								inQualifiedSection = false;
							}
						}

						// In the case of sections, we err on the side of using the section name, even if there
						// are invalid characters after.  We do still check, though, to issue a warning.
						pos3 = line.find_first_not_of(' ', pos2 + 1);
						if (pos3 != std::string::npos)
						{
							if (line.at(pos3) != ';')
							{
								EP_WARN("File System: Invalid characters encountered after section name in INI file \"{}\"."
								"  Characters after ']' were discarded.  Line:{}", path, ini.CurrentLine());
							}
						}
					}
					else
					{
						EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
								"  Line:{}", path, ini.CurrentLine());
					}

				}
				else if (line.at(pos) != ';' && inQualifiedSection)
				{
					pos2 = line.find_first_of(" =", pos);

					if (pos == std::string::npos)
					{
						// Delimiter is missing.
						EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
								"  Line:{}", path, ini.CurrentLine());
					}
					else if (pos2 > line.find_first_of(';', pos))
					{
						// Delimiter was detected, but a semicolon cuts it off.
						EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
								"  Line:{}", path, ini.CurrentLine());
					}
					else
					{
						// This appears to be a key.
						key = HN(line.substr(pos, pos2 - pos));

						pos = line.find_first_not_of(' ', pos2);
						if (pos == std::string::npos)
						{
							// Delimiter is missing.
							EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
									"  Line:{}", path, ini.CurrentLine());
						}
						else if (line.at(pos) == '=')
						{
							// We've got the delimiter.
							pos++;
							if (pos >= line.size())
							{
								// The delimiter was the last character on the line.
								// This may have been intentional, so we add an empty string to the map.
								m_data[section][key].emplace_back(std::string());
							}
							else
							{
								pos = line.find_first_not_of(' ', pos);
								if (pos == std::string::npos)
								{
									// The delimiter was the last thing on the line.
									// This may have been intentional, so we add an empty string to the map.
									m_data[section][key].emplace_back(std::string());
								}
								else if (line.at(pos) == ';')
								{
									// A semicolon immediately follows the delimiter.
									// This may have been intentional, so we add an empty string to the map.
									m_data[section][key].emplace_back(std::string());
								}
								else if (line.at(pos) == '\"')
								{
									// Quoted value, spaces are allowed.
									pos2 = line.find_first_of('\"', pos + 1);
									if (pos2 == std::string::npos)
									{
										// Unpaired quotes
										EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
												"  Line:{}", path, ini.CurrentLine());
									}
									else
									{
										// Found the closing quotes.
										// Check for unexpected characters after.
										pos3 = line.find_first_not_of(' ', pos2 + 1);
										if (pos3 == std::string::npos)
										{
											// Good: no characters after quotes.
											m_data[section][key].emplace_back(line.substr(pos + 1, pos2 - pos - 1));
										}
										else if (line.at(pos3) == ';')
										{
											// Good: only a comment after the quotes
											m_data[section][key].emplace_back(line.substr(pos + 1, pos2 - pos - 1));
										}
										else
										{
											// Bad: unexpected characters after quotes.
											EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
													"  Line:{}", path, ini.CurrentLine());
										}
									}
								}
								else if (line.at(pos) == '(')
								{
									// Dictionary value: allow spaces between parenthesis.
									// We must ensure we're not responding to characters in quotes.
									pos2 = line.find_first_of("\");", pos);
									bool inQuotes = false;
									while (pos2 != std::string::npos)
									{
										if (line.at(pos2) == '\"')
										{
											inQuotes = !inQuotes;
										}
										else if (!inQuotes)
										{
											if (line.at(pos2) == ';')
											{
												// Problem!  The semicolon cuts off the right parenthesis.
												pos2 = std::string::npos;
												break;
											}
											else if (line.at(pos2) == ')')
											{
												// We've reached the right parenthesis legitimately.
												break;
											}
										}
										pos2 = line.find_first_of("\");", pos2 + 1);
									}

									if (pos2 == std::string::npos)
									{
										// The right parentheses is either missing, in quotes, or cut off by a semicolon.
										EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
												"  Line:{}", path, ini.CurrentLine());
									}
									else
									{
										// Pos2 is currently the position of the right parenthesis.
										// Check for unexpected characters after it.
										pos3 = line.find_first_not_of(' ', pos2 + 1);
										if (pos3 == std::string::npos)
										{
											// Good: no characters after ')'
											m_data[section][key].emplace_back(line.substr(pos, pos2 + 1 - pos));
										}
										else if (line.at(pos3) == ';')
										{
											// Good: only a comment after ')'
											m_data[section][key].emplace_back(line.substr(pos, pos2 + 1 - pos));
										}
										else
										{
											// Bad: unexpected characters after ')'.
											EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
													"  Line:{}", path, ini.CurrentLine());
										}
									}
								}
								else
								{
									// Regular value.
									pos2 = line.find_first_of(" ;", pos);

									if (pos2 == std::string::npos)
									{
										// All good!  No characters follow the value.
										m_data[section][key].emplace_back(line.substr(pos, pos2 - pos));
									}
									else if (line.at(pos2) == ';')
									{
										// All good!  Semicolon cuts off remaining characters after value.
										m_data[section][key].emplace_back(line.substr(pos, pos2 - pos));
									}
									else
									{
										pos3 = line.find_first_not_of(' ', pos2);
										if (pos3 == std::string::npos)
										{
											// All good!  All whitespace after the value.
											m_data[section][key].emplace_back(line.substr(pos, pos2 - pos));
										}
										else if (line.at(pos3) == ';')
										{
											// All good!  Only comments after the value.
											m_data[section][key].emplace_back(line.substr(pos, pos2 - pos));
										}
										else
										{
											// No! There are illegal characters after the value.
											EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
													"  Line:{}", path, ini.CurrentLine());
										}
									}
								}
							}
						}
						else
						{
							// Unexpected character when looking for delimiter.
							EP_WARN("File System: Invalid line encountered in INI file \"{}\".  Line was skipped."
									"  Line:{}", path, ini.CurrentLine());
						}
					}
				}
			}
		}
	}
	else if (areErrorsFatal)
	{
		EP_FATAL("File System: A required INI file cannot be opened.  The game cannot continue."
					"  File: {}  ErrorCode: {}", path, ErrorCodeToString(ini.GetError()));
		throw Exceptions::FatalError();
	}

	return m_errorcode;
}


bool File::INIReader::GetBool(HashName section, HashName key, bool defaultVal)
{
	if (m_data[section][key].size() == 0)
	{
		EP_WARN("File System: INIReader::GetBool() called on a key not loaded from the INI file.  "
				"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		return defaultVal;
	}
	else
	{
		std::pair<bool, bool> converted = INIStringToBool(m_data[section][key].front());

		if (!converted.first)
		{
			EP_WARN("File System: An INI bool value was discarded due to a conversion issue.  "
					"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));

			return defaultVal;
		}
		else if (m_data[section][key].size() > 1)
		{
			EP_WARN("File System: INIReader::GetBool() called on a key with multiple values.  "
					"Only the first was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		}

		return converted.second;
	}
}


std::vector<bool> File::INIReader::GetMultiBool(HashName section, HashName key)
{
	std::vector<bool> returnVal;

	if (m_data[section][key].size() != 0)
	{
		std::pair<bool, bool> converted;
		for (std::string& strIt : m_data[section][key])
		{
			converted = INIStringToBool(strIt);
			if (converted.first)
			{
				returnVal.push_back(converted.second);
			}
			else
			{
				EP_WARN("File System: An INI bool value was discarded due to a conversion issue."
						"\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
			}
		}
	}

	return returnVal;
}


int File::INIReader::GetInt(HashName section, HashName key, int defaultVal)
{
	if (m_data[section][key].size() == 0)
	{
		EP_WARN("File System: INIReader::GetInt() called on a key not loaded from the INI file.  "
				"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));

		return defaultVal;
	}
	else
	{
		std::pair<bool, int> converted = INIStringToInt(m_data[section][key].front());

		if (!converted.first)
		{
			EP_WARN("File System: An INI int value was discarded due to a conversion issue.  "
					"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));

			return defaultVal;
		}
		else if (m_data[section][key].size() > 1)
		{
			EP_WARN("File System: INIReader::GetInt() called on a key with multiple values.  "
					"Only the first was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		}

		return converted.second;
	}
}


std::vector<int> File::INIReader::GetMultiInt(HashName section, HashName key)
{
	std::vector<int> returnVal;

	if (m_data[section][key].size() != 0)
	{
		std::pair<bool, int> converted;

		for (std::string& strIt : m_data[section][key])
		{
			converted = INIStringToInt(strIt);

			if (converted.first)
			{
				returnVal.push_back(converted.second);
			}
			else
			{
				EP_WARN("File System: An INI int value was discarded due to a conversion issue."
						"\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
			}
		}
	}

	return returnVal;
}


float File::INIReader::GetFloat(HashName section, HashName key, float defaultVal)
{
	if (m_data[section][key].size() == 0)
	{
		EP_WARN("File System: INIReader::GetFloat() called on a key not loaded from the INI file.  "
				"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		return defaultVal;
	}
	else
	{
		std::pair<bool, float> converted = INIStringToFloat(m_data[section][key].front());

		if (!converted.first)
		{
			EP_WARN("File System: An INI float value was discarded due to a conversion issue.  "
					"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
			return defaultVal;
		}
		else if (m_data[section][key].size() > 1)
		{
			EP_WARN("File System: INIReader::GetFloat() called on a key with multiple values.  "
					"Only the first was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		}

		return converted.second;
	}
}


std::vector<float> File::INIReader::GetMultiFloat(HashName section, HashName key)
{
	std::vector<float> returnVal;
	
	if (m_data[section][key].size() != 0)
	{
		std::pair<bool, float> converted;

		for (std::string& strIt : m_data[section][key])
		{
			converted = INIStringToFloat(strIt);
			if (converted.first)
			{
				returnVal.push_back(converted.second);
			}
			else
			{
				EP_WARN("File System: An INI float value was discarded due to a conversion issue."
						"\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
			}
		}
	}

	return returnVal;
}


std::string File::INIReader::GetStr(HashName section, HashName key, std::string defaultVal)
{
	if (m_data[section][key].size() == 0)
	{
		EP_WARN("File System: INIReader::GetStr() called on a key not loaded from the INI file.  "
				"The default value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		return defaultVal;
	}
	else if (m_data[section][key].size() > 1)
	{
		EP_WARN("File System: INIReader::GetStr() called on a key with multiple values.  "
				"Only the first was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
	}

	return m_data[section][key].front();
}


std::vector<std::string> File::INIReader::GetMultiStr(HashName section, HashName key)
{
	std::vector<std::string> returnVal;

	if (m_data[section][key].size() != 0)
	{
		for (std::string& strIt : m_data[section][key])
		{
			returnVal.push_back(strIt);
		}
	}

	return returnVal;
}


std::unordered_map<HashName, std::string> File::INIReader::GetDictionary(HashName section, HashName key)
{
	std::unordered_map<HashName, std::string> returnVal;

	if (m_data[section][key].size() == 0)
	{
		EP_WARN("File System: INIReader::GetDictionary() called on a key not loaded from the INI file.  "
				"An empty unordered_map was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
	}
	else
	{
		returnVal = INIStringToDictionary(m_data[section][key].front());

		if (returnVal.empty())
		{
			EP_WARN("File System: An INI dictionary value was discarded due to a parsing issue.  "
					"An empty unordered_map was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		}
		else if (m_data[section][key].size() > 1)
		{
			EP_WARN("File System: INIReader::GetDictionary() called on a key with multiple values.  "
					"Only the first value was returned.\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
		}
	}

	return returnVal;
}


std::vector<std::unordered_map<HashName, std::string>> File::INIReader::GetMultiDictionary(HashName section, HashName key)
{
	std::vector<std::unordered_map<HashName, std::string>> returnVal;

	if (m_data[section][key].size() != 0)
	{
		std::unordered_map<HashName, std::string> dict;

		for (const std::string& str : m_data[section][key])
		{
			dict = INIStringToDictionary(str);

			if (dict.empty())
			{
				EP_WARN("File System: An INI dictionary value was discarded due to a parsing issue."
						"\nFile: {}\nSection: {}\nKey: {}", m_path, HN_ToStr(section), HN_ToStr(key));
			}
			else
			{
				returnVal.push_back(dict);
			}
		}
	}

	return returnVal;
}

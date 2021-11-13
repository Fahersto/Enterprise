#include "EP_PCH.h"
#include "Graphics.h"
#include "Enterprise/File/File.h"

#include "OpenGLHelpers.h"

using Enterprise::Graphics;

// TODO: Overhaul this file with EP_WASSERT for warning checks.

static std::map<HashName, std::string> includeSrcStrings;

HashName Graphics::activeShaderName = HN("EPNULLSHADER");
HashName Graphics::fallbackShaderName = HN_NULL;
std::set<HashName> Graphics::activeShaderOptions;
std::set<HashName> Graphics::fallbackShaderOptions;

static std::map<HashName, std::map<std::set<HashName>, GLuint>> oglShaderPrograms; // Key 1: Program name, Key 2: Options
GLuint oglActiveProgram = 0; // Accessed in VertexArrays.cpp via external linkage
static std::map<HashName, int> shaderRefCount;
static std::map<HashName, HashName> shaderSourcePaths;
static HashName currentShaderSourcePath = HN_NULL;

// Uniform buffer stuff
std::map<Graphics::UniformBufferHandle, size_t> Graphics::uniformBufferSizes;
std::map<HashName, std::map<HashName, size_t>> Graphics::uniformBlockSizes; // Key 1: Program name Key 2: Block name
static std::map<Graphics::UniformBufferHandle, HashName> uniformBlockNames;

static std::map<HashName, GLuint> uniformBlockBindingPoints;
static GLuint nextBindingPoint = 0;

// Sampler uniform stuff
// Key 1: Program name Key 2: Options Key 3: Uniform name
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, GLint>>> samplerUniformTypes;
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, GLint>>> samplerUniformLocations;

// Vertex buffer stuff
// Key 1: Program name, Key 2: Options, Key 3: Attribute name
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, unsigned int>>> Graphics::shaderAttributeLocations;
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, int>>> Graphics::shaderAttributeArrayLengths;
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, Graphics::ShaderDataType>>> Graphics::shaderAttributeTypes;


// Shaders

bool Graphics::CompileShaderSrc(const std::string& src)
{
	std::string shaderName;

	// String constants (inserted into shader source during preprocessing)
	const GLchar* universalStringBuffer = "#version 410\n*//*#define \nEP_VERTEX\nEP_FRAGMENT\n";
	//                                   |0            | | |17    |24|26        |36          |
	GLchar* versionStr = (char*)universalStringBuffer + 0;
	constexpr GLint versionLength = 13;
	GLchar* closeCommentStr = (char*)universalStringBuffer + 13;
	constexpr GLint closeCommentLength = 2;
	GLchar* openCommentStr = (char*)universalStringBuffer + 15;
	constexpr GLint openCommentLength = 2;
	GLchar* defineStr = (char*)universalStringBuffer + 17;
	constexpr GLint defineLength = 8; // Space at end is shared with spaceStr
	GLchar* spaceStr = (char*)universalStringBuffer + 24;
	constexpr GLint spaceLength = 1;
	GLchar* newlineStr = (char*)universalStringBuffer + 25;
	constexpr GLint newlineLength = 1;
	GLchar* vertexdefineStr = (char*)universalStringBuffer + 26;
	constexpr GLint vertexdefineLength = 10;
	GLchar* fragmentdefineStr = (char*)universalStringBuffer + 36;
	constexpr GLint fragmentdefineLength = 12;

	// Buffers of string pointers and string lengths to pass to glShaderSource()
	std::vector<GLchar*>  oglVertexShaderSourceStrings   = { versionStr, defineStr, vertexdefineStr };
	std::vector<GLint>    oglVertexShaderSourceLengths   = { versionLength, defineLength, vertexdefineLength };
	std::vector<GLchar*>  oglFragmentShaderSourceStrings = { versionStr, defineStr, fragmentdefineStr };
	std::vector<GLint>    oglFragmentShaderSourceLengths = { versionLength, defineLength, fragmentdefineLength };

	// Pragma option string substitution
	std::vector<size_t> pragmaOptionStringPositions;
	std::vector<int> pragmaOptionStringLengths;
	std::vector<std::vector<size_t>> pragmaOptionStringNameIndices; // Ordered by "#pragma" line no., then option names l to r
	std::vector<int> pragmaOptionStringInsertionPoints;

	// Used to handle string length calculation upon encountering a shader section directive
	std::vector<char*>* currentShaderSourceStrings = nullptr;
	std::vector<int>* currentShaderSourceLengths = nullptr;

	// Parser state tracking
	std::string_view line;
	size_t lineNumber = 0;
	int parseState = 0;
	bool inMultilineComment = false;

	for (size_t charsToLineStart = 0;
		charsToLineStart < src.length();
		charsToLineStart += line.length() + 1)
	{
		lineNumber++;
		line = std::string_view(src.data() + charsToLineStart,
			src.find_first_of("\n\0", charsToLineStart) - charsToLineStart);

		for (size_t cursor = 0; cursor != std::string_view::npos; )
		{
			switch (parseState)
			{
			case 0: // Looking for "#epshader" line
			{
				if (!inMultilineComment)
				{
					cursor = line.find_first_not_of(" \t\r", cursor);
					if (cursor != std::string_view::npos)
					{
						if (line[cursor] == '/')
						{
							if (line.length() > cursor + 1)
							{
								if (line[cursor + 1] == '/')
								{
									// End of line comment ("//")
									cursor = std::string_view::npos;
								}
								else if (line[cursor + 1] == '*')
								{
									// Start of multiline comment ("/*")
									cursor += 2;
									inMultilineComment = true;
								}
								else
								{
									// First token starts with '/'
									EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  Shader source does not begin with \"#epshader\".  "
										"Line: {}", lineNumber);
									return false;
								}
							}
							else
							{
								// First token is '/', the only token on the line
								EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  Shader source does not begin with \"#epshader\".  "
									"Line: {}", lineNumber);
								return false;
							}
						}
						else if (line[cursor] == '#')
						{
							if (line.compare(cursor, 9, "#epshader") == 0)
							{
								cursor += 9;
								if (line.length() > cursor)
								{
									if (line[cursor] == '/')
									{
										if (line.length() > cursor + 1)
										{
											if (line[cursor + 1] != '*')
											{
												// Either this is a "//" (in which case there's erroneously no token on the rest of the line)
												// or a non-comment '/' immediately following "#epshader".
												EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" is not followed by "
													"a program name.  Line: {}", lineNumber);
												return false;
											}
											// else "#epshader" is immediately followed by "/*", which is handled in the next parseState.
										}
										else
										{
											// '/' immediately following #epshader, last character on the line
											EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" followed by "
												"a non-whitespace character.  Line: {}", lineNumber);
											return false;
										}
									}
									else if (line[cursor] != ' ' && line[cursor] != '\t')
									{
										// Non-whitespace immediately after "#epshader"
										EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" followed by "
											"a non-whitespace character.  Line: {}", lineNumber);
										return false;
									}
								}
								else
								{
									// "#epshader" is last token on line
									EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" is not followed by "
										"a program name.  Line: {}", lineNumber);
									return false;
								}
								parseState = 1;
							}
							else
							{
								// '#' is the start of an invalid token
								EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  Shader source does not begin with \"#epshader\".  "
									"Line: {}", lineNumber);
								return false;
							}
						}
						else
						{
							// Invalid token
							EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  Shader source does not begin with \"#epshader\".  "
								"Line: {}", lineNumber);
							return false;
						}
					}
				}
				else // inMultilineComment
				{
					cursor = line.find('*', cursor);
					if (cursor != std::string_view::npos)
					{
						cursor++;
						if (line.length() > cursor)
						{
							if (line[cursor] == '/')
							{
								// Comment closure detected
								inMultilineComment = false;
								cursor++;
							}
						}
						else
						{
							// Star was last character in line, and not part of "*/"
							cursor = std::string_view::npos;
						}
					}
					else
					{
						// Didn't find comment closure on this line
						cursor = std::string_view::npos;
					}
				}
			}
			break;

			case 1: // Looking for shader name (starts with cursor at character immediately after "#epshader")
			{
				// TODO: Add support for quoted program names
				if (!inMultilineComment)
				{
					cursor = line.find_first_not_of(" \t\r", cursor);
					if (cursor != std::string_view::npos)
					{
						if (line[cursor] == '/')
						{
							if (line.length() > cursor + 1)
							{
								if (line[cursor + 1] == '/')
								{
									// End of line comment ("//")
									EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
										"\"#epshader\" is not followed by a program name.  "
										"Line: {}", lineNumber);
									return false;
								}
								else if (line[cursor + 1] == '*')
								{
									// Start of multiline comment ("/*")
									inMultilineComment = true;
									cursor += 2;
								}
								else
								{
									// '/' was start of valid token
									goto startofvalidtoken;
								}
							}
							else
							{
								// Valid one-character name (that just happens to be "/")
								shaderName = "/";

								// Reference counting
								if (shaderRefCount.count(HN("/")) == 0)
								{
									shaderRefCount[HN("/")] = 1;
									shaderSourcePaths[HN("/")] = currentShaderSourcePath;
								}
								else
								{
									shaderRefCount[HN("/")]++;
									if (shaderSourcePaths[HN("/")] != currentShaderSourcePath)
									{
										EP_WARN("Graphics::CompileShaderSrc(): Shader program \"/\" was previously compiled from "
											"a different source location!  Check shader source files for name collisions.  "
											"First Path: {}, Second Path: {}", shaderSourcePaths[HN("/")], currentShaderSourcePath);
									}
									return true;
								}

								cursor = std::string_view::npos;
								parseState = 2;

								// Mark the start of the global section in the shader source arrays
								oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
								oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
							}
						}
						else
						{
							// Start of valid token
						startofvalidtoken: // Used to jump out of previous if block if program name starts with '/'.

							for (size_t subcursor = cursor; subcursor != std::string_view::npos; )
							{
								subcursor = line.find_first_of(" \t\r/", subcursor);
								if (subcursor != std::string_view::npos)
								{
									if (line[subcursor] == '/')
									{
										if (line.length() > subcursor + 1)
										{
											if (line[subcursor + 1] == '/')
											{
												// End of line comment
												shaderName = line.substr(cursor, subcursor - cursor);
												HashName hn = HN(shaderName);

												// Reference counting
												if (shaderRefCount.count(hn) == 0)
												{
													shaderRefCount[hn] = 1;
													shaderSourcePaths[hn] = currentShaderSourcePath;
												}
												else
												{
													shaderRefCount[hn]++;
													if (shaderSourcePaths[hn] != currentShaderSourcePath)
													{
														EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" was previously compiled from "
															"a different source location!  Check shader source files for name collisions.  "
															"First Path: {}, Second Path: {}",
															shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
													}
													return true;
												}

												cursor = std::string_view::npos;
												parseState = 2;

												// Mark the start of the global section in the shader source arrays
												oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
												oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
												break;
											}
											else if (line[subcursor + 1] == '*')
											{
												// Start of multiline comment
												inMultilineComment = true;
												shaderName = line.substr(cursor, subcursor - cursor);
												HashName hn = HN(shaderName);

												// Reference counting
												if (shaderRefCount.count(hn) == 0)
												{
													shaderRefCount[hn] = 1;
													shaderSourcePaths[hn] = currentShaderSourcePath;
												}
												else
												{
													shaderRefCount[hn]++;
													if (shaderSourcePaths[hn] != currentShaderSourcePath)
													{
														EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" was previously compiled from "
															"a different source location!  Check shader source files for name collisions.  "
															"First Path: {}, Second Path: {}",
															shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
													}
													return true;
												}

												cursor = std::string_view::npos;
												parseState = 2;
												subcursor += 2;

												// Mark the start of the global section in the shader source arrays
												oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
												oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);

												// Continue to parse for unexpected tokens or multiline comments
												while (subcursor != std::string_view::npos)
												{
													if (!inMultilineComment)
													{
														subcursor = line.find_first_not_of(" \t\r", subcursor);
														if (subcursor != std::string_view::npos)
														{
															if (line[subcursor] == '/')
															{
																subcursor++;
																if (line.length() > subcursor)
																{
																	if (line[subcursor] == '/')
																	{
																		// End of line comment
																		break;
																	}
																	else if (line[subcursor] == '*')
																	{
																		// Start of multiline comment
																		inMultilineComment = true;
																		subcursor++;
																	}
																	else
																	{
																		// '/' was not part of a comment
																		EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after shader program name.  "
																			"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
																		subcursor = std::min(line.find("//", subcursor), line.find("/*", subcursor));
																	}
																}
																else
																{
																	// Not enough room for '/' to be start of a comment.
																	EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after shader program name.  "
																		"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
																	break;
																}
															}
															else
															{
																// Unexpected token
																EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after shader program name.  "
																	"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
																subcursor = std::min(line.find("//", subcursor), line.find("/*", subcursor));
															}
														}
													}
													else // inMultilineComment
													{
														subcursor = line.find('*', subcursor);
														if (subcursor != std::string_view::npos)
														{
															subcursor++;
															if (line.length() > subcursor)
															{
																if (line[subcursor] == '/')
																{
																	inMultilineComment = false;
																	subcursor++;
																}
															}
														}
													}
												}

												break;
											}
											else
											{
												// Just a slash.  Potentially part of valid program name, so we continue
												subcursor++;
											}
										}
										else
										{
											// '/' is last character of program name and source line
											shaderName = line.substr(cursor);
											HashName hn = HN(shaderName);

											// Reference counting
											if (shaderRefCount.count(hn) == 0)
											{
												shaderRefCount[hn] = 1;
												shaderSourcePaths[hn] = currentShaderSourcePath;
											}
											else
											{
												shaderRefCount[hn]++;
												if (shaderSourcePaths[hn] != currentShaderSourcePath)
												{
													EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" was previously compiled from "
														"a different source location!  Check shader source files for name collisions.  "
														"First Path: {}, Second Path: {}", shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
												}
												return true;
											}

											cursor = std::string_view::npos;
											parseState = 2;

											// Mark the start of the global section in the shader source arrays
											oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
											oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
											break;
										}
									}
									else
									{
										// Valid program name terminated by whitespace
										shaderName = line.substr(cursor, subcursor - cursor);
										HashName hn = HN(shaderName);

										// Reference counting
										if (shaderRefCount.count(hn) == 0)
										{
											shaderRefCount[hn] = 1;
											shaderSourcePaths[hn] = currentShaderSourcePath;
										}
										else
										{
											shaderRefCount[hn]++;
											if (shaderSourcePaths[hn] != currentShaderSourcePath)
											{
												EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" was previously compiled from "
													"a different source location!  Check shader source files for name collisions.  "
													"First Path: {}, Second Path: {}", shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
											}
											return true;
										}

										cursor = std::string_view::npos;
										parseState = 2;

										// Mark the start of the global section in the shader source arrays
										oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
										oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);

										// Continue to parse for unexpected tokens or multiline comments
										while (subcursor != std::string_view::npos)
										{
											if (!inMultilineComment)
											{
												subcursor = line.find_first_not_of(" \t\r", subcursor);
												if (subcursor != std::string_view::npos)
												{
													if (line[subcursor] == '/')
													{
														subcursor++;
														if (line.length() > subcursor)
														{
															if (line[subcursor] == '/')
															{
																// End of line comment
																break;
															}
															else if (line[subcursor] == '*')
															{
																// Start of multiline comment
																inMultilineComment = true;
																subcursor++;
															}
															else
															{
																// '/' was not part of a comment
																EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after shader program name.  "
																	"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
																subcursor = std::min(line.find("//", subcursor), line.find("/*", subcursor));
															}
														}
														else
														{
															// Not enough room for '/' to be start of a comment.
															EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after shader program name.  "
																"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
															break;
														}
													}
													else
													{
														// Unexpected token
														EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after shader program name.  "
															"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
														subcursor = std::min(line.find("//", subcursor), line.find("/*", subcursor));
													}
												}
											}
											else // inMultilineComment
											{
												subcursor = line.find('*', subcursor);
												if (subcursor != std::string_view::npos)
												{
													subcursor++;
													if (line.length() > subcursor)
													{
														if (line[subcursor] == '/')
														{
															inMultilineComment = false;
															subcursor++;
														}
													}
												}
											}
										}

										break;
									}
								}
								else
								{
									// Valid token terminated by EOL
									shaderName = line.substr(cursor);
									HashName hn = HN(shaderName);

									// Reference counting
									if (shaderRefCount.count(hn) == 0)
									{
										shaderRefCount[hn] = 1;
										shaderSourcePaths[hn] = currentShaderSourcePath;
									}
									else
									{
										shaderRefCount[hn]++;
										if (shaderSourcePaths[hn] != currentShaderSourcePath)
										{
											EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" was previously compiled from "
												"a different source location!  Check shader source files for name collisions.  "
												"First Path: {}, Second Path: {}", shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
										}
										return true;
									}

									cursor = std::string_view::npos;
									parseState = 2;

									// Mark the start of the global section in the shader source arrays
									oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
									oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
								}
							}
						}
					}
					else
					{
						// No token after "#epshader"
						EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" is not followed by a program name.  "
							"Line: {}", lineNumber);
						return false;
					}
				}
				else // inMultilineComment
				{
					cursor = line.find('*', cursor);
					if (cursor != std::string_view::npos)
					{
						cursor++;
						if (line.length() > cursor)
						{
							if (line[cursor] == '/')
							{
								// End of multi-line comment
								inMultilineComment = false;
								cursor++;
							}
						}
						else
						{
							// Line not long enough to contain "*/"
							EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" is not followed by a program name.  "
								"Line: {}", lineNumber);
							return false;
						}
					}
					else
					{
						// No token after "#epshader"
						EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  \"#epshader\" is not followed by a program name.  "
							"Line: {}", lineNumber);
						return false;
					}
				}
			}
			break;
			case 2: // Parse global section for "#include", "#pragma option", and shader section directives
			{
				if (!inMultilineComment)
				{
					cursor = line.find_first_of("#/", cursor);
					if (cursor != std::string_view::npos)
					{
						if (line[cursor] == '#')
						{
							if (line.compare(cursor, 7, "#pragma") == 0)
							{
								cursor += 7;
								if (line.length() > cursor + 7)
								{
									if (line[cursor] == ' ' || line[cursor] == '\t' || line.compare(cursor, 2, "/*") == 0)
									{
										// "#pragma" is properly terminated by either whitespace or a multiline comment.

										bool isPragmaOption = false;

										// While loop for looking for "option".
										while (cursor != std::string_view::npos)
										{
											if (!inMultilineComment)
											{
												cursor = line.find_first_not_of(" \t\r", cursor);
												if (cursor != std::string_view::npos)
												{
													if (line[cursor] == '/')
													{
														cursor++;
														if (line.length() > cursor)
														{
															if (line[cursor] == '/')
															{
																// end of line comment
																cursor = std::string_view::npos;
															}
															else if (line[cursor] == '*')
															{
																// multiline comment
																inMultilineComment = true;
																cursor++;
															}
														}
													}
													else if (line.compare(cursor, 6, "option") == 0)
													{
														// "#pragma option" confirmed!
														pragmaOptionStringNameIndices.emplace_back();
														cursor += 6;
														isPragmaOption = true;
														break;
													}
													else
													{
														// Some other "#pragma"
														// Parse for multiline comments until cursor == npos
														while (cursor != std::string_view::npos)
														{
															if (inMultilineComment)
															{
																cursor = line.find('/', cursor);
																if (cursor != std::string_view::npos)
																{
																	cursor++;
																	if (line.length() > cursor)
																	{
																		if (line[cursor] == '/')
																		{
																			cursor = std::string_view::npos;
																		}
																		else if (line[cursor] == '*')
																		{
																			inMultilineComment = true;
																			cursor++;
																		}
																	}
																}
															}
															else // !inMultilineComment
															{
																cursor = line.find("*/", cursor);
																if (cursor != std::string_view::npos)
																{
																	inMultilineComment = false;
																	cursor += 2;
																}
															}
														}
													}
												}
											}
											else // inMultilineComment
											{
												cursor = line.find('*', cursor);
												if (cursor != std::string_view::npos)
												{
													cursor++;
													if (line.length() > cursor)
													{
														if (line[cursor] == '/')
														{
															inMultilineComment = false;
															cursor++;
														}
													}
												}
											}
										}

										// Look for option names (if not "#pragma option", cursor == npos already)
										while (cursor != std::string_view::npos)
										{
											if (!inMultilineComment)
											{
												cursor = line.find_first_not_of(" \t\r", cursor);
												if (cursor != std::string_view::npos)
												{
													if (line[cursor] == '/')
													{
														if (line.length() > cursor + 1)
														{
															if (line[cursor + 1] == '/')
															{
																// end of line comment.
																cursor = std::string_view::npos;
																break;
															}
															else if (line[cursor + 1] == '*')
															{
																// start of multiline comment.
																inMultilineComment = true;
																cursor += 2;
															}
															else
															{
																// Invalid token start.
																EP_WARN("Graphics::CompileShaderSrc(): Invalid variant option name!  "
																	"Token begins with a forward slash.  Program: {}, Line: {}", shaderName, lineNumber);
																cursor = std::min({ line.find_first_of(" \t\r", cursor), line.find("//", cursor), line.find("/*") });
															}
														}
													}
													else
													{
														// TODO: Enforce alphanumeric variant option names
														
														// Valid token start.
														size_t subcursor = line.find_first_of(" \t\r/", cursor);
														if (subcursor != std::string_view::npos)
														{
															if (line[subcursor] == '/')
															{
																if (line.length() > subcursor + 1)
																{
																	if (line[subcursor + 1] == '/')
																	{
																		// Valid token, terminated by end of line comment
																		pragmaOptionStringNameIndices.back().emplace_back(pragmaOptionStringPositions.size());
																		pragmaOptionStringPositions.emplace_back(charsToLineStart + cursor);
																		pragmaOptionStringLengths.emplace_back(subcursor - cursor);

																		cursor = subcursor + 2;
																	}
																	else if (line[subcursor + 1] == '*')
																	{
																		// Valid token, terminated by multiline comment
																		pragmaOptionStringNameIndices.back().emplace_back(pragmaOptionStringPositions.size());
																		pragmaOptionStringPositions.emplace_back(charsToLineStart + cursor);
																		pragmaOptionStringLengths.emplace_back(subcursor - cursor);

																		cursor = subcursor + 2;
																		inMultilineComment = true;
																	}
																	else
																	{
																		// Invalid token, contains forward slash.
																		EP_WARN("Graphics::CompileShaderSrc(): Invalid variant option name!  "
																			"Token contains a forward slash.  Program: {}, Line: {}", shaderName, lineNumber);
																		cursor = std::min(
																			{
																				line.find_first_of(" \t\r", subcursor),
																				line.find("//", subcursor),
																				line.find("/*", subcursor)
																			});
																	}
																}
															}
															else
															{
																// Valid token, terminated by whitespace
																pragmaOptionStringNameIndices.back().emplace_back(pragmaOptionStringPositions.size());
																pragmaOptionStringPositions.emplace_back(charsToLineStart + cursor);
																pragmaOptionStringLengths.emplace_back(subcursor - cursor);
																cursor = subcursor + 1;
															}
														}
														else
														{
															// Valid token, terminated by end of line.
															pragmaOptionStringNameIndices.back().emplace_back(pragmaOptionStringPositions.size());
															pragmaOptionStringPositions.emplace_back(charsToLineStart + cursor);
															pragmaOptionStringLengths.emplace_back(line.length() - cursor);

															cursor = std::string_view::npos;
														}
													}
												}
											}
											else // inMultilineComment
											{
												cursor = line.find('*', cursor);
												if (cursor != std::string_view::npos)
												{
													cursor++;
													if (line.length() > cursor)
													{
														if (line[cursor] == '/')
														{
															inMultilineComment = false;
															cursor++;
														}
													}
												}
											}
										}

										if (isPragmaOption)
										{
											// Terminate the current source string
											oglVertexShaderSourceLengths.emplace_back(
												charsToLineStart + line.length() + 1 - (oglVertexShaderSourceStrings.back() - src.data()));
											oglFragmentShaderSourceLengths.emplace_back(
												charsToLineStart + line.length() + 1 - (oglFragmentShaderSourceStrings.back() - src.data()));

											// Close off multiline comment for insertion purposes
											if (inMultilineComment)
											{
												oglVertexShaderSourceStrings.emplace_back(closeCommentStr);
												oglVertexShaderSourceLengths.emplace_back(closeCommentLength);
												oglFragmentShaderSourceStrings.emplace_back(closeCommentStr);
												oglFragmentShaderSourceLengths.emplace_back(closeCommentLength);
											}

											// Add "#define "
											oglVertexShaderSourceStrings.emplace_back(defineStr);
											oglVertexShaderSourceLengths.emplace_back(defineLength);
											oglFragmentShaderSourceStrings.emplace_back(defineStr);
											oglFragmentShaderSourceLengths.emplace_back(defineLength);

											// Add the insertion point
											pragmaOptionStringInsertionPoints.emplace_back(oglVertexShaderSourceLengths.size());
											oglVertexShaderSourceLengths.emplace_back(0);
											oglVertexShaderSourceStrings.emplace_back(nullptr);
											oglFragmentShaderSourceLengths.emplace_back(0);
											oglFragmentShaderSourceStrings.emplace_back(nullptr);

											// Add newline
											oglVertexShaderSourceStrings.emplace_back(newlineStr);
											oglVertexShaderSourceLengths.emplace_back(newlineLength);
											oglFragmentShaderSourceStrings.emplace_back(newlineStr);
											oglFragmentShaderSourceLengths.emplace_back(newlineLength);

											// Reopen multiline comment
											if (inMultilineComment)
											{
												oglVertexShaderSourceStrings.emplace_back(openCommentStr);
												oglVertexShaderSourceLengths.emplace_back(openCommentLength);
												oglFragmentShaderSourceStrings.emplace_back(openCommentStr);
												oglFragmentShaderSourceLengths.emplace_back(openCommentLength);
											}

											// Start the new source string
											oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
											oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
										}
									}
								}
							}
							else if (line.compare(cursor, 8, "#include") == 0)
							{
								cursor = line.find_first_not_of(" \t\r", cursor + 8);
								if (cursor != std::string_view::npos)
								{
									if (line[cursor] == '<' || line[cursor] == '\"')
									{
										size_t subcursor = line.find_first_of(line[cursor] == '<' ? ">/" : "\"/", cursor + 1);
										for (; subcursor != std::string_view::npos; )
										{
											if (!inMultilineComment)
											{
												if (line[subcursor] == '>' || line[subcursor] == '"')
												{
													std::string includePath;
													if (line[subcursor] == '>')
													{
														includePath = File::engineShadersPath + std::string(line.substr(cursor + 1, subcursor - cursor - 1));
													}
													else // line[subcursor] == '"'
													{
														includePath = std::string(line.substr(cursor + 1, subcursor - cursor - 1));
													}

													HashName includePathHash = HN(includePath);
													if (!includeSrcStrings.count(includePathHash))
													{
														if (File::Exists(includePath))
														{
															if (File::LoadTextFile(includePath, &includeSrcStrings[includePathHash])
																!= File::ErrorCode::Success)
															{
																EP_WARN("Graphics::CompileShaderSource(): Could not load include file \"{}\".  "
																	"Program: {}, Line: {}", includePath, shaderName, lineNumber);
																cursor = line.find_first_of('/', subcursor + 1);
																for (; cursor != std::string_view::npos; )
																{
																	if (!inMultilineComment)
																	{
																		if (line.length() > cursor + 1)
																		{
																			if (line[cursor + 1] == '/')
																			{
																				break;
																			}
																			else if (line[cursor + 1] == '*')
																			{
																				inMultilineComment = true;
																				cursor = line.find_first_of('*', cursor + 2);
																			}
																		}
																	}
																	else if (line[cursor] == '*')
																	{
																		if (line.length() > cursor + 1)
																		{
																			if (line[cursor + 1] == '/')
																			{
																				inMultilineComment = false;
																				cursor = line.find_first_of('/', cursor + 2);
																			}
																		}
																	}
																}
																break;
															}
														}
														else
														{
															EP_WARN("Graphics::CompileShaderSource(): Include file \"{}\" does not exist!  "
																"Program: {}, Line: {}", includePath, shaderName, lineNumber);
															break;
														}
													}

													// Insert the include file

													// Terminate the current source string
													oglVertexShaderSourceLengths.emplace_back(
														charsToLineStart - (oglVertexShaderSourceStrings.back() - src.data())
													);
													oglFragmentShaderSourceLengths.emplace_back(
														charsToLineStart - (oglFragmentShaderSourceStrings.back() - src.data())
													);

													// Close off multiline comment for insertion purposes
													if (inMultilineComment)
													{
														oglVertexShaderSourceStrings.emplace_back(closeCommentStr);
														oglVertexShaderSourceLengths.emplace_back(closeCommentLength);
														oglFragmentShaderSourceStrings.emplace_back(closeCommentStr);
														oglFragmentShaderSourceLengths.emplace_back(closeCommentLength);
													}

													// Add contents of include file
													oglVertexShaderSourceStrings.emplace_back(includeSrcStrings[includePathHash].data());
													oglVertexShaderSourceLengths.emplace_back(includeSrcStrings[includePathHash].length());
													oglFragmentShaderSourceStrings.emplace_back(includeSrcStrings[includePathHash].data());
													oglFragmentShaderSourceLengths.emplace_back(includeSrcStrings[includePathHash].length());

													// Add newline
													oglVertexShaderSourceStrings.emplace_back(newlineStr);
													oglVertexShaderSourceLengths.emplace_back(newlineLength);
													oglFragmentShaderSourceStrings.emplace_back(newlineStr);
													oglFragmentShaderSourceLengths.emplace_back(newlineLength);

													// Reopen multiline comment
													if (inMultilineComment)
													{
														oglVertexShaderSourceStrings.emplace_back(openCommentStr);
														oglVertexShaderSourceLengths.emplace_back(openCommentLength);
														oglFragmentShaderSourceStrings.emplace_back(openCommentStr);
														oglFragmentShaderSourceLengths.emplace_back(openCommentLength);
													}

													// Start the new source string
													oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
													oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);

													break;
												}
												else if (line.length() > subcursor + 1) // Must be '/'.
												{
													if (line[subcursor + 1] == '*')
													{
														inMultilineComment = true;
														subcursor = line.find_first_of('*', subcursor + 2);
													}
													else if (line[subcursor + 1] == '/')
													{
														EP_WARN("Graphics::CompileShaderSource(): \"#include\" followed by illegitimate string.  "
															"Program: {}, Line:{}", shaderName, lineNumber);
														break;
													}
												}
												else
												{
													EP_WARN("Graphics::CompileShaderSource(): \"#include\" followed by illegitimate string.  "
														"Program: {}, Line:{}", shaderName, lineNumber);
													break;
												}
											}
											else if (subcursor != std::string_view::npos)
											{
												if (line.length() > subcursor + 1)
												{
													if (line[subcursor + 1] == '/')
													{
														inMultilineComment = false;
														subcursor = line.find_first_of(line[cursor] == '<' ? ">/" : "\"/", subcursor + 2);
													}
													else
													{
														subcursor = line.find_first_of('*', subcursor + 2);
													}
												}
											}
										}
									}
									else
									{
										EP_WARN("Graphics::CompileShaderSource(): \"#include\" followed by illegitimate string.  "
											"Program: {}, Line:{}", shaderName, lineNumber);
									}
								}
							}
							else if (line.compare(cursor, 7, "#vertex") == 0)
							{
								// Terminate the current source strings to just before "#vertex"
								oglVertexShaderSourceLengths.emplace_back(charsToLineStart + cursor - (oglVertexShaderSourceStrings.back() - src.data()));
								oglFragmentShaderSourceLengths.emplace_back(charsToLineStart + cursor - (oglFragmentShaderSourceStrings.back() - src.data()));

								// Continue parsing for multiline comments
								for (cursor = line.find_first_not_of(" \t\r", cursor + 7); cursor != std::string_view::npos; )
								{
									if (!inMultilineComment)
									{
										if (line[cursor] == '/')
										{
											cursor++;
											if (line.length() > cursor)
											{
												if (line[cursor] == '/')
												{
													// End-of-line comment, safe to move on
													cursor = std::string_view::npos;
													break;
												}
												else if (line[cursor] == '*')
												{
													// Start of mult-line comment
													inMultilineComment = true;
													cursor = line.find_first_of('*', cursor + 1);
												}
												else
												{
													// Slash was the start of an invalid token.
													EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#vertex\" statement.  "
														"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
													cursor = line.find_first_of(" \t\r/", cursor);
												}
											}
											else
											{
												// Slash was the start of an invalid token, and last character on line
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#vertex\" statement.  "
													"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
												cursor = std::string_view::npos;
											}
										}
										else
										{
											// unexpected
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#vertex\" statement.  "
												"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
											cursor = line.find_first_of(" \t\r/", cursor);
										}
									}
									else // inMultilineComment
									{
										cursor++;
										if (line.length() > cursor)
										{
											if (line[cursor] == '/')
											{
												// Multi-line comment terminated
												inMultilineComment = false;
												cursor = line.find_first_not_of(" \t\r", cursor + 1);
											}
										}
									}
								}

								// Switch to parsing for vertex shader
								oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
								currentShaderSourceStrings = &oglVertexShaderSourceStrings;
								currentShaderSourceLengths = &oglVertexShaderSourceLengths;
								parseState = 3;
							}
							else if (line.compare(cursor, 9, "#fragment") == 0)
							{
								// Terminate the current source strings to just before "#fragment"
								oglVertexShaderSourceLengths.emplace_back(
									charsToLineStart + cursor - (oglVertexShaderSourceStrings.back() - src.data()));
								oglFragmentShaderSourceLengths.emplace_back(
									charsToLineStart + cursor - (oglFragmentShaderSourceStrings.back() - src.data()));

								// Continue parsing for multiline comments
								for (cursor = line.find_first_not_of(" \t\r", cursor + 9); cursor != std::string_view::npos; )
								{
									if (!inMultilineComment)
									{
										if (line[cursor] == '/')
										{
											cursor++;
											if (line.length() > cursor)
											{
												if (line[cursor] == '/')
												{
													// End-of-line comment, safe to move on
													cursor = std::string_view::npos;
													break;
												}
												else if (line[cursor] == '*')
												{
													// Start of mult-line comment
													inMultilineComment = true;
													cursor = line.find_first_of('*', cursor + 1);
												}
												else
												{
													// Slash was the start of an invalid token.
													EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#fragment\" statement.  "
														"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
													cursor = line.find_first_of(" \t\r/", cursor);
												}
											}
											else
											{
												// Slash was the start of an invalid token, and last character on line
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#fragment\" statement.  "
													"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
												cursor = std::string_view::npos;
											}
										}
										else
										{
											// unexpected
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#fragment\" statement.  "
												"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
											cursor = line.find_first_of(" \t\r/", cursor);
										}
									}
									else // inMultilineComment
									{
										cursor++;
										if (line.length() > cursor)
										{
											if (line[cursor] == '/')
											{
												// Multi-line comment terminated
												inMultilineComment = false;
												cursor = line.find_first_not_of(" \t\r", cursor + 1);
											}
										}
									}
								}

								// Switch to parsing for fragment shader
								oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
								currentShaderSourceStrings = &oglFragmentShaderSourceStrings;
								currentShaderSourceLengths = &oglFragmentShaderSourceLengths;
								parseState = 3;
							}
							else
							{
								// '#' is not part of "#include", "#pragma", or a shader section directive.
								cursor++;
							}
						}
						else // line[cursor] == '/'
						{
							cursor++;
							if (line.length() > cursor)
							{
								if (line[cursor] == '/')
								{
									cursor = std::string_view::npos;
								}
								else if (line[cursor] == '*')
								{
									inMultilineComment = true;
									cursor++;
								}
							}
						}
					}
				}
				else // inMultilineComment
				{
					cursor = line.find('*', cursor);
					if (cursor != std::string_view::npos)
					{
						cursor++;
						if (line.length() > cursor)
						{
							if (line[cursor] == '/')
							{
								inMultilineComment = false;
								cursor++;
							}
						}
					}
				}
			}
			break;
			case 3: // Shader section parsing
			{
				if (!inMultilineComment)
				{
					cursor = line.find_first_of("/#", cursor);
					if (cursor != std::string_view::npos)
					{
						if (line[cursor] == '/')
						{
							cursor++;
							if (line.length() > cursor)
							{
								if (line[cursor] == '/')
								{
									cursor = std::string_view::npos;
								}
								else if (line[cursor] == '*')
								{
									inMultilineComment = true;
									cursor++;
								}
							}
						}
						else if (line.compare(cursor, 7, "#vertex") == 0)
						{
							// Terminate the current source strings to just before "#vertex"
							currentShaderSourceLengths->emplace_back(
								charsToLineStart + cursor - (oglVertexShaderSourceStrings.back() - src.data()));

							// Continue parsing for multiline comments
							for (cursor = line.find_first_not_of(" \t\r", cursor + 7); cursor != std::string_view::npos; )
							{
								if (!inMultilineComment)
								{
									if (line[cursor] == '/')
									{
										cursor++;
										if (line.length() > cursor)
										{
											if (line[cursor] == '/')
											{
												// End-of-line comment, safe to move on
												cursor = std::string_view::npos;
												break;
											}
											else if (line[cursor] == '*')
											{
												// Start of mult-line comment
												inMultilineComment = true;
												cursor = line.find_first_of('*', cursor + 1);
											}
											else
											{
												// Slash was the start of an invalid token.
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#vertex\" statement.  "
													"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
												cursor = line.find_first_of(" \t\r/", cursor);
											}
										}
										else
										{
											// Slash was the start of an invalid token, and last character on line
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#vertex\" statement.  "
												"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
											cursor = std::string_view::npos;
										}
									}
									else
									{
										// unexpected
										EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#vertex\" statement.  "
											"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
										cursor = line.find_first_of(" \t\r/", cursor);
									}
								}
								else // inMultilineComment
								{
									cursor++;
									if (line.length() > cursor)
									{
										if (line[cursor] == '/')
										{
											// Multi-line comment terminated
											inMultilineComment = false;
											cursor = line.find_first_not_of(" \t\r", cursor + 1);
										}
									}
								}
							}

							// Switch to parsing for vertex shader
							oglVertexShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
							currentShaderSourceStrings = &oglVertexShaderSourceStrings;
							currentShaderSourceLengths = &oglVertexShaderSourceLengths;
							parseState = 3;
						}
						else if (line.compare(cursor, 9, "#fragment") == 0)
						{
							// Terminate the current source strings to just before "#fragment"
							currentShaderSourceLengths->emplace_back(
								charsToLineStart + cursor - (oglVertexShaderSourceStrings.back() - src.data()));

							// Continue parsing for multiline comments
							for (cursor = line.find_first_not_of(" \t\r", cursor + 9); cursor != std::string_view::npos; )
							{
								if (!inMultilineComment)
								{
									if (line[cursor] == '/')
									{
										cursor++;
										if (line.length() > cursor)
										{
											if (line[cursor] == '/')
											{
												// End-of-line comment, safe to move on
												cursor = std::string_view::npos;
												break;
											}
											else if (line[cursor] == '*')
											{
												// Start of mult-line comment
												inMultilineComment = true;
												cursor = line.find_first_of('*', cursor + 1);
											}
											else
											{
												// Slash was the start of an invalid token.
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#fragment\" statement.  "
													"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
												cursor = line.find_first_of(" \t\r/", cursor);
											}
										}
										else
										{
											// Slash was the start of an invalid token, and last character on line
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#fragment\" statement.  "
												"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
											cursor = std::string_view::npos;
										}
									}
									else
									{
										// unexpected
										EP_WARN("Graphics::CompileShaderSrc(): Unexpected token after \"#fragment\" statement.  "
											"Token has been ignored.  Program: {}, Line: {}", shaderName, lineNumber);
										cursor = line.find_first_of(" \t\r/", cursor);
									}
								}
								else // inMultilineComment
								{
									cursor++;
									if (line.length() > cursor)
									{
										if (line[cursor] == '/')
										{
											// Multi-line comment terminated
											inMultilineComment = false;
											cursor = line.find_first_not_of(" \t\r", cursor + 1);
										}
									}
								}
							}

							// Switch to parsing for fragment shader
							oglFragmentShaderSourceStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
							currentShaderSourceStrings = &oglFragmentShaderSourceStrings;
							currentShaderSourceLengths = &oglFragmentShaderSourceLengths;
							parseState = 3;
						}
						else
						{
							cursor++;
						}
					}
				}
				else // inMultilineComment
				{
					cursor = line.find('*', cursor);
					if (cursor != std::string_view::npos)
					{
						cursor++;
						if (line.length() > cursor)
						{
							if (line[cursor] == '/')
							{
								inMultilineComment = false;
								cursor++;
							}
						}
					}
				}
			}
			break;
			default:
				EP_ASSERT_NOENTRY();
				break;
			}
		}
	}

	// Terminate final string with -1 (causes glShaderSource() to interpret last string as null-terminated)
	EP_ASSERT(currentShaderSourceStrings);
	if (currentShaderSourceStrings->size() > currentShaderSourceLengths->size())
		currentShaderSourceLengths->emplace_back(-1);

	// At this point, every buffer has been filled for this file.
	// We just need to create all permutations by substituting the "#pragma option" statements and compile them.

	// Calculate the number of variants
	int numOfVariants = 1;
	for (int i = 0; i < pragmaOptionStringNameIndices.size(); i++)
	{
		numOfVariants *= pragmaOptionStringNameIndices[i].size();
	}

	GLint oglStringOutBufferSize = 50;
	GLchar* oglStringOutBuffer = (GLchar*)alloca(oglStringOutBufferSize * sizeof(GLchar));

	GLsizei oglUniformListsSize = 0;
	GLuint* oglUniformListIndices = nullptr; // Simple list of all integers between 0 and oglUniformListsSize.
	GLint* oglUniformBlockIndices = nullptr;
	GLint* oglUniformTypes = nullptr;

	// Assemble and compile all shader variants
	for (int i = 0; i < numOfVariants; i++) // For every variant
	{
		size_t divvalue = 1;
		std::set<HashName> variantLookupKey;

		// Assemble shader variants
		for (int j = 0; j < pragmaOptionStringNameIndices.size(); j++) // For each #pragma option statement
		{
			size_t modvalue = pragmaOptionStringNameIndices[j].size();
			// j is the index of the pragma statement
			// (i / divvalue % modvalue) is the index for this variant's option

			// Selectively handle null option name ("_")
			if (src[pragmaOptionStringPositions[pragmaOptionStringNameIndices[j][i / divvalue % modvalue]]] == '_'
				&& pragmaOptionStringLengths[pragmaOptionStringNameIndices[j][i / divvalue % modvalue]] == 1)
			{
				oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j] - (GLint)1] = spaceStr;
				oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j] - (GLint)1] = spaceLength;
				oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j]] = spaceStr;
				oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j]] = spaceLength;

				oglFragmentShaderSourceStrings[pragmaOptionStringInsertionPoints[j] - (GLint)1] = spaceStr;
				oglFragmentShaderSourceLengths[pragmaOptionStringInsertionPoints[j] - (GLint)1] = spaceLength;
				oglFragmentShaderSourceStrings[pragmaOptionStringInsertionPoints[j]] = spaceStr;
				oglFragmentShaderSourceLengths[pragmaOptionStringInsertionPoints[j]] = spaceLength;
			}
			else
			{
				oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j] - (GLint)1] = defineStr;
				oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j] - (GLint)1] = defineLength;
				oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j]] =
					(GLchar*)src.data() + pragmaOptionStringPositions[pragmaOptionStringNameIndices[j][i / divvalue % modvalue]];
				oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j]] =
					pragmaOptionStringLengths[pragmaOptionStringNameIndices[j][i / divvalue % modvalue]];

				oglFragmentShaderSourceStrings[pragmaOptionStringInsertionPoints[j] - (GLint)1] = defineStr;
				oglFragmentShaderSourceLengths[pragmaOptionStringInsertionPoints[j] - (GLint)1] = defineLength;
				oglFragmentShaderSourceStrings[pragmaOptionStringInsertionPoints[j]] =
					oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j]];
				oglFragmentShaderSourceLengths[pragmaOptionStringInsertionPoints[j]] =
					oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j]];

				auto keyinsertresult = variantLookupKey.emplace(HN(oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j]],
					oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j]]));

				if (!keyinsertresult.second)
				{
					EP_WARN("Graphics::CompileShaderSrc(): A shader option is defined twice in a single shader variant.  "
						"Program: {}, Option: {}", shaderName,
						std::string(oglVertexShaderSourceStrings[pragmaOptionStringInsertionPoints[j]],
							oglVertexShaderSourceLengths[pragmaOptionStringInsertionPoints[j]]));
				}
			}

			divvalue *= modvalue;
		}

		// Compilation and linking
		if (oglShaderPrograms[HN(shaderName)].count(variantLookupKey) == 0)
		{
			// Create shader objects
			EP_ASSERT_SLOW(oglVertexShaderSourceStrings.size() == oglVertexShaderSourceLengths.size());
			EP_ASSERT_SLOW(oglFragmentShaderSourceStrings.size() == oglFragmentShaderSourceLengths.size());

			// Upload source code
			GLuint vshader = EP_GL(glCreateShader(GL_VERTEX_SHADER));
			GLuint fshader = EP_GL(glCreateShader(GL_FRAGMENT_SHADER));
			EP_GL(glShaderSource(vshader,
				(GLsizei)oglVertexShaderSourceStrings.size(),
				oglVertexShaderSourceStrings.data(),
				oglVertexShaderSourceLengths.data()));
			EP_GL(glShaderSource(fshader,
				(GLsizei)oglFragmentShaderSourceStrings.size(),
				oglFragmentShaderSourceStrings.data(),
				oglFragmentShaderSourceLengths.data()));

			// Compile Vertex
			GLint compilationResult;
			EP_GL(glCompileShader(vshader));
			EP_GL(glGetShaderiv(vshader, GL_COMPILE_STATUS, &compilationResult));

			if (compilationResult == GL_FALSE)
			{
				GLint length;
				EP_GL(glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &length));
				GLchar* message = (GLchar*)alloca(length * sizeof(GLchar));
				EP_GL(glGetShaderInfoLog(vshader, length, &length, message));
				EP_ERROR("[OpenGL] Vertex shader compilation failure! \n{}", message);
				{
					// Print pre-processsed source
					// TODO: Use better error reporting mechanism to log this in the future
					std::string_view view;
					std::cout << "Variant " << i + 1 << ": " << std::endl;
					std::cout << "Vertex shader source: " << std::endl;
					for (int j = 0; j < oglVertexShaderSourceStrings.size(); j++)
					{
						if (oglVertexShaderSourceLengths[j] >= 0)
						{
							view = std::string_view(oglVertexShaderSourceStrings[j], oglVertexShaderSourceLengths[j]);
						}
						else
						{
							view = std::string_view(oglVertexShaderSourceStrings[j]);
						}
						std::cout << view;
					}
				}

				EP_GL(glDeleteShader(vshader));
				return false;
			}

			// Compile Fragment
			EP_GL(glCompileShader(fshader));
			EP_GL(glGetShaderiv(fshader, GL_COMPILE_STATUS, &compilationResult));

			if (compilationResult == GL_FALSE)
			{
				GLint length;
				EP_GL(glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &length));
				GLchar* message = (GLchar*)alloca(length * sizeof(GLchar));
				EP_GL(glGetShaderInfoLog(fshader, length, &length, message));
				EP_ERROR("[OpenGL] Fragment shader compilation failure! \n{}", message);
				{
					// Print pre-processsed source
					// TODO: Use better error reporting mechanism to log this in the future
					std::string_view view;
					std::cout << "Variant " << i + 1 << ": " << std::endl;
					std::cout << "Fragment shader source: " << std::endl;
					for (int j = 0; j < oglFragmentShaderSourceStrings.size(); j++)
					{
						if (oglFragmentShaderSourceLengths[j] >= 0)
						{
							view = std::string_view(oglFragmentShaderSourceStrings[j], oglFragmentShaderSourceLengths[j]);
						}
						else
						{
							view = std::string_view(oglFragmentShaderSourceStrings[j]);
						}
						std::cout << view;
					}
				}

				EP_GL(glDeleteShader(vshader));
				EP_GL(glDeleteShader(fshader));
				return false;
			}

			// Link program
			GLuint program = EP_GL(glCreateProgram());
			EP_GL(glAttachShader(program, vshader));
			EP_GL(glAttachShader(program, fshader));
			EP_GL(glLinkProgram(program));
			EP_GL(glDetachShader(program, vshader));
			EP_GL(glDetachShader(program, fshader));
			EP_GL(glDeleteShader(vshader));
			EP_GL(glDeleteShader(fshader));

			EP_GL(glGetProgramiv(program, GL_LINK_STATUS, &compilationResult));
			if (compilationResult == GL_FALSE)
			{
				GLint length;
				EP_GL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);)
					GLchar* message = (GLchar*)alloca(length * sizeof(GLchar));
				EP_GL(glGetProgramInfoLog(program, length, &length, message));
				EP_ERROR("[OpenGL] Shader program linking error! {}", message);
				EP_GL(glDeleteProgram(program));
				return false;
			}

			oglShaderPrograms[HN(shaderName)][variantLookupKey] = program;


			// Introspection
			// TODO: Move this outside of the compilation loop to reduce implicit pipeline flushes
			GLint maxNameLength, count, size;

			// Uniform buffers
			EP_GL(glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxNameLength));
			if (maxNameLength > oglStringOutBufferSize)
			{
				oglStringOutBuffer = (GLchar*)alloca(maxNameLength * sizeof(GLchar));
				oglStringOutBufferSize = maxNameLength;
			}

			EP_GL(glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count));
			for (int i = 0; i < count; i++)
			{
				// Get block deets
				GLsizei length;
				EP_GL(glGetActiveUniformBlockName(program, (GLuint)i, maxNameLength, &length, oglStringOutBuffer)); // Block name
				EP_GL(glGetActiveUniformBlockiv(program, (GLuint)i, GL_UNIFORM_BLOCK_DATA_SIZE, &size)); // Block size
				HashName hn = HN(oglStringOutBuffer, length);

				// Store size
				if (uniformBlockSizes[HN(shaderName)].count(hn) == 0)
					uniformBlockSizes[HN(shaderName)][hn] = size;
				else
				{
					EP_ASSERTF(uniformBlockSizes[HN(shaderName)][hn] == size,
						"Graphics::CompileShaderSrc(): Definition of uniform block differs between shader variants.  "
						"Uniform blocks are required to use std140 layout.");
				}

				// Bind block
				if (uniformBlockBindingPoints.count(hn) == 0)
				{
					EP_GL(glUniformBlockBinding(program, i, nextBindingPoint));
					uniformBlockBindingPoints[hn] = nextBindingPoint;
					nextBindingPoint++;
					EP_ASSERT(nextBindingPoint < maxUniformBindingPoints);
				}
				else
				{
					EP_GL(glUniformBlockBinding(program, i, uniformBlockBindingPoints[hn]));
				}
			}

			// Sampler uniforms
			EP_GL(glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength));
			if (maxNameLength > oglStringOutBufferSize)
			{
				oglStringOutBuffer = (GLchar*)alloca(maxNameLength * sizeof(GLchar));
				oglStringOutBufferSize = maxNameLength;
			}
			EP_GL(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count));
			if (count > oglUniformListsSize)
			{
				oglUniformListIndices = (GLuint*)alloca(count * sizeof(GLuint));
				for (unsigned int i = 0; i < count; i++)
				{
					oglUniformListIndices[i] = i;
				}
				oglUniformBlockIndices = (GLint*)alloca(count * sizeof(GLint));
				oglUniformTypes = (GLint*)alloca(count * sizeof(GLint));
				oglUniformListsSize = count;
			}

			EP_GL(glGetActiveUniformsiv(program, count, oglUniformListIndices, GL_UNIFORM_BLOCK_INDEX, oglUniformBlockIndices));
			EP_GL(glGetActiveUniformsiv(program, count, oglUniformListIndices, GL_UNIFORM_TYPE, oglUniformTypes));
			for (unsigned int i = 0; i < count; i++)
			{
				if (oglUniformBlockIndices[i] == -1) // Not part of a block.
				{
					if ((oglUniformTypes[i] >= GL_SAMPLER_1D 				&& oglUniformTypes[i] <= GL_SAMPLER_2D_SHADOW) ||
						(oglUniformTypes[i] >= GL_SAMPLER_1D_ARRAY 			&& oglUniformTypes[i] <= GL_SAMPLER_CUBE_SHADOW) ||
						(oglUniformTypes[i] >= GL_INT_SAMPLER_1D 			&& oglUniformTypes[i] <= GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) ||
						(oglUniformTypes[i] >= GL_SAMPLER_2D_RECT 			&& oglUniformTypes[i] <= GL_UNSIGNED_INT_SAMPLER_BUFFER) ||
						(oglUniformTypes[i] >= GL_SAMPLER_2D_MULTISAMPLE 	&& oglUniformTypes[i] <= GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY))
					{
						GLsizei nameLength;
						EP_GL(glGetActiveUniformName(program, i, oglStringOutBufferSize, &nameLength, oglStringOutBuffer));

						samplerUniformTypes[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, nameLength)] = oglUniformTypes[i];
						samplerUniformLocations[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, nameLength)] =
							EP_GL(glGetUniformLocation(program, oglStringOutBuffer));
					}
					else
					{
						EP_ERROR("Graphics::CompileShaderSrc(): Shader program contains non-sampler uniform!  "
							"Program: {}, Uniform: {}", shaderName, oglStringOutBuffer);
					}
				}
			}

			// Vertex attributes
			EP_GL(glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength));
			if (maxNameLength > oglStringOutBufferSize)
			{
				oglStringOutBuffer = (GLchar*)alloca(maxNameLength * sizeof(GLchar));
				oglStringOutBufferSize = maxNameLength;
			}
			EP_GL(glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count));
			for (int i = 0; i < count; i++)
			{
				GLint length;
				GLenum type;
				glGetActiveAttrib(program, i, oglStringOutBufferSize, &length, &size, &type, oglStringOutBuffer);

				shaderAttributeLocations[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, length)] = glGetAttribLocation(program, oglStringOutBuffer);
				shaderAttributeArrayLengths[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, length)] = size;
				shaderAttributeTypes[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, length)] = oglTypeToShaderDataType(type);
			}
		}
		else
		{
			// Equivalent program is already compiled.
			EP_WARN("Graphics::CompileShaderSrc(): \"#pragma option\" statements result in equivalent variants.  "
				"Duplicate compilation skipped.  Program: {}", shaderName);
		}
	}

	return true;
}

bool Graphics::LoadShaderFile(std::string path)
{
	std::string src;
	if (File::LoadTextFile(path, &src) == File::ErrorCode::Success)
	{
		currentShaderSourcePath = HN(path);
		if (CompileShaderSrc(src))
		{
			currentShaderSourcePath = HN_NULL;
			return true;
		}
		else
		{
			EP_ERROR("Graphics::LoadShaderFile(): Error compiling shader!  File: \"{}\"", path);
			currentShaderSourcePath = HN_NULL;
			return false;
		}
	}
	else
	{
		EP_ERROR("Graphics::LoadShaderFile(): Error loading shader file!  File: \"{}\"", path);
		return false;
	}
}

void Graphics::DeleteShader(HashName program)
{
	if (oglShaderPrograms.count(program) == 0)
	{
		EP_WARN("Graphics::DeleteShader(): No shader named \"{}\" has been loaded.", HN_ToStr(program));
		return;
	}

	shaderRefCount[program]--;
	if (shaderRefCount[program] <= 0)
	{
		if (activeShaderName == program)
		{
			activeShaderName = HN("EPNULLSHADER");
			oglActiveProgram = oglShaderPrograms[HN("EPNULLSHADER")][{}];;
		}

		for (const auto& [options, variant] : oglShaderPrograms[program])
		{
			EP_GL(glDeleteProgram(variant));
		}
		oglShaderPrograms.erase(program);
		shaderAttributeLocations.erase(program);
		shaderAttributeArrayLengths.erase(program);
		shaderAttributeTypes.erase(program);
	}
}


void Graphics::EnableShaderOption(HashName option)
{
	if (!activeShaderOptions.emplace(option).second)
		EP_WARN("Graphics::EnableShaderOption(): Shader option already enabled!  Option: {}", option);
}

void Graphics::DisableShaderOption(HashName option)
{
	if (activeShaderOptions.erase(option) == 0)
		EP_WARN("Graphics::DisableShaderOption(): Shader option not enabled!  Option: {}", option);
}

bool Graphics::isShaderViable(HashName shader)
{
	if (oglShaderPrograms.count(shader) > 0)
	{
		return oglShaderPrograms[shader].count(activeShaderOptions) > 0;
	}
	else
	{
		EP_WARN("Graphics::isShaderViable(): No shader program exists with provided HashName.  Program: {}", shader);
		return false;
	}
}


void Graphics::SetFallbackShader(HashName shader, std::set<HashName> options)
{
	if (fallbackShaderName != HN_NULL)
	{
		EP_WARN("Graphics::SetFallbackShader(): A fallback shader is already set!  Old: {}, New: {}", fallbackShaderName, shader);
	}

	if (oglShaderPrograms.count(shader) > 0)
	{
		if (oglShaderPrograms[shader].count(options) > 0)
		{
			fallbackShaderName = shader;
			fallbackShaderOptions = options;
		}
		else
		{
			EP_ERROR("Graphics::SetFallbackShader(): Shader program does not support provided shader options.  "
				"Program: {}, Options: {}", shader, options);
		}
	}
	else
	{
		EP_ERROR("Graphics::SetFallbackShader(): No shader program exists with the provided HashName.  "
			"Fallback shader remains unset.  Program: {}", shader);
	}
}

void Graphics::UnsetFallbackShader()
{
	if (fallbackShaderName == HN_NULL)
	{
		EP_WARN("Graphics::UnsetFallbackShader(): Shader fallback is already unset.");
	}
	fallbackShaderName = HN_NULL;
	fallbackShaderOptions.clear();
}


void Graphics::BindShader(HashName program)
{
	if (isShaderViable(program))
	{
		activeShaderName = program;
		if (oglActiveProgram != oglShaderPrograms[activeShaderName][activeShaderOptions])
		{
			oglActiveProgram = oglShaderPrograms[activeShaderName][activeShaderOptions];
			EP_GL(glUseProgram(oglActiveProgram));
		}
	}
	else
	{
		if (fallbackShaderName != HN_NULL)
		{
			activeShaderName = fallbackShaderName;
			if (isShaderViable(fallbackShaderName))
			{
				if (oglActiveProgram != oglShaderPrograms[fallbackShaderName][activeShaderOptions])
					EP_GL(glUseProgram(oglShaderPrograms[fallbackShaderName][activeShaderOptions]));
			}
			else
			{
				EP_WARN("Graphics::BindShader(): Fallback shader is not viable with current shader options."
					"Using fallback options.  "
					"Fallback Program: {}, Active Shader Options: {}", fallbackShaderName, activeShaderOptions);

				// This combination is guaranteed viable as it was checked in Graphics::SetFallbackShader().
				EP_GL(glUseProgram(oglShaderPrograms[fallbackShaderName][fallbackShaderOptions]));
			}
		}
		else
		{
			activeShaderName = HN("EPNULLSHADER");
			EP_WARN("Graphics::BindShader(): Unable to bind shader program and no fallback program has been set.  "
				"Program: {}", program);

			oglActiveProgram = oglShaderPrograms[HN("EPNULLSHADER")][{}];
			EP_GL(glUseProgram(oglActiveProgram));
		}
	}
}


Graphics::UniformBufferHandle Graphics::CreateUniformBuffer(HashName name, size_t size, void* data, bool dynamic)
{
	EP_ASSERTF(size > 0, "Graphics::CreateUniformBuffer(): 'size' cannot be 0");
	EP_ASSERTF(size < maxUniformBufferSize, "Graphics::CreateUniformBuffer(): 'size' exceeds limit set by OpenGL implementation.");

	GLuint buffer = 0;
	EP_GL(glGenBuffers(1, &buffer));
	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, buffer));
	EP_GL(glBufferData(GL_UNIFORM_BUFFER, size, data, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	EP_ASSERTF(buffer != 0, "Graphics::CreateUniformBuffer(): Generation of uniform buffer failed!");

	uniformBufferSizes[buffer] = size;
	uniformBlockNames[buffer] = name;
	if (uniformBlockBindingPoints.count(name) == 0)
	{
		EP_GL(glBindBufferBase(GL_UNIFORM_BUFFER, nextBindingPoint, buffer));
		uniformBlockBindingPoints[name] = nextBindingPoint;
		nextBindingPoint++;
		EP_ASSERT(nextBindingPoint < maxUniformBindingPoints);
	}
	else
	{
		EP_GL(glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBindingPoints[name], buffer));
	}

	return buffer;
}

void Graphics::DeleteUniformBuffer(UniformBufferHandle buffer)
{
	EP_ASSERTF(uniformBufferSizes.count(buffer) != 0, "Graphics::DeleteUniformBuffer(): 'buffer' is not a valid handle.");

	EP_GL(glDeleteBuffers(1, &buffer));
	uniformBufferSizes.erase(buffer);
	uniformBlockNames.erase(buffer);
}


void Graphics::SetUniformBufferData(UniformBufferHandle buffer, void* data)
{
	EP_ASSERTF(uniformBufferSizes.count(buffer) != 0, "Graphics::SetUniformBufferData(): 'buffer' is not a valid handle.");

	// When replacing the entire buffer, we simply orphan it, which reduces implicit synchronization.
	// https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, buffer));
	EP_GL(glBufferData(GL_UNIFORM_BUFFER, uniformBufferSizes[buffer], nullptr, GL_STREAM_DRAW));
	EP_GL(glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBufferSizes[buffer], data));
	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
void Graphics::SetUniformBufferData(UniformBufferHandle buffer, void* data, size_t offset, size_t count)
{
	EP_ASSERTF(uniformBufferSizes.count(buffer) != 0, "Graphics::SetUniformBufferData(): 'buffer' is not a valid handle.");
	EP_ASSERTF(offset + count <= uniformBufferSizes[buffer], "Graphics::SetUniformBufferData(): Copied data would exceed buffer bounds.");

	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, buffer));
	EP_GL(glBufferSubData(GL_UNIFORM_BUFFER, offset, count, data));
	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void Graphics::BindUniformBuffer(UniformBufferHandle buffer)
{
	EP_ASSERTF(uniformBufferSizes.count(buffer) != 0, "Graphics::SetUniformBufferData(): 'buffer' is not a valid handle.");
	EP_GL(glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBindingPoints[uniformBlockNames[buffer]], buffer));
}

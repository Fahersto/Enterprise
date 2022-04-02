#include "Enterprise/Graphics/Graphics.h"
#include "Enterprise/File/File.h"

#include "Enterprise/Graphics/OpenGLHelpers.h"

using Enterprise::Graphics;
#define EP_DUMP_GLSL

// TODO: Overhaul this file with EP_WASSERT for warning checks.

HashName Graphics::selectedShaderName = HN("EPNULLSHADER");
HashName Graphics::activeShaderName = HN("EPNULLSHADER");
HashName Graphics::fallbackShaderName = HN_NULL;
std::set<HashName> Graphics::selectedShaderOptions;
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
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, std::vector<GLint>>>> samplerUniformLocations;

// Vertex buffer stuff
// Key 1: Program name, Key 2: Options, Key 3: Attribute name
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, unsigned int>>> Graphics::shaderAttribLocations;
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, int>>> Graphics::shaderAttribArrayLengths;
std::map<HashName, std::map<std::set<HashName>, std::map<HashName, ShaderDataType>>> Graphics::shaderAttribTypes;

// Include file stuff
static std::map<HashName, std::string> inclSrcBuffers;
static std::map<HashName, std::vector<GLchar*>> inclSrcStrings;
static std::map<HashName, std::vector<GLint>> inclSrcLengths;

static std::map<HashName, std::vector<size_t>> inclPragmaOptStrPositions;
static std::map<HashName, std::vector<int>> inclPragmaOptStrLengths;
// Key: hash of include path.  Vectors ordered by "#pragma" line no., then option index
static std::map<HashName, std::vector<std::vector<size_t>>> inclPragmaOptStrNameIndices;
static std::map<HashName, std::vector<int>> inclPragmaOptStrInsPoints;

// Shader file parsing

// String constants (inserted into shader source during preprocessing)
static const GLchar* universalStringBuffer = "#version 410\n*//*#define \nEP_VERTEX\nEP_FRAGMENT\n";
//                                            |0            | | |17    |24|26        |36          |
static GLchar* versionStr = (char*)universalStringBuffer + 0;
static constexpr GLint versionLength = 13;
static GLchar* closeCommentStr = (char*)universalStringBuffer + 13;
static constexpr GLint closeCommentLength = 2;
static GLchar* openCommentStr = (char*)universalStringBuffer + 15;
static constexpr GLint openCommentLength = 2;
static GLchar* defineStr = (char*)universalStringBuffer + 17;
static constexpr GLint defineLength = 8; // Space at end is shared with spaceStr
static GLchar* spaceStr = (char*)universalStringBuffer + 24;
static constexpr GLint spaceLength = 1;
static GLchar* newlineStr = (char*)universalStringBuffer + 25;
static constexpr GLint newlineLength = 1;
static GLchar* vertexdefineStr = (char*)universalStringBuffer + 26;
static constexpr GLint vertexdefineLength = 10;
static GLchar* fragmentdefineStr = (char*)universalStringBuffer + 36;
static constexpr GLint fragmentdefineLength = 12;


static void parseIncludeFile(HashName inclPathHN)
{
	std::string_view line;
	size_t lineNumber = 0;
	bool inMultilineComment = false;

	inclSrcStrings[inclPathHN].emplace_back(inclSrcBuffers[inclPathHN].data());

	for (size_t charsToLineStart = 0;
		charsToLineStart < inclSrcBuffers[inclPathHN].length();
		charsToLineStart += line.length() + 1)
	{
		lineNumber++;
		line = std::string_view(inclSrcBuffers[inclPathHN].data() + charsToLineStart,
			std::min(inclSrcBuffers[inclPathHN].find_first_of('\n', charsToLineStart),
				inclSrcBuffers[inclPathHN].length()) - charsToLineStart);

		size_t pos = line.find_first_not_of(" \t\r", 0);
		if (pos != std::string_view::npos)
		{
			if (line.compare(pos, 7, "#pragma") == 0)
			{
				pos = line.find_first_not_of(" \t\r", pos + 7);
				if (pos != std::string_view::npos)
				{
					if (line.compare(pos, 6, "option") == 0)
					{
						pos += 7;

						if (pos < inclSrcBuffers[inclPathHN].length())
						{
							if (line[pos - 1] == ' ' || line[pos - 1] == '\t' || line[pos - 1] == '\r')
							{
								inclPragmaOptStrNameIndices[inclPathHN].emplace_back();

								// Look for option names (if not "#pragma option", pos == npos already)
								while (pos != std::string_view::npos)
								{
									if (!inMultilineComment)
									{
										pos = line.find_first_not_of(" \t\r", pos);
										if (pos != std::string_view::npos)
										{
											if (line[pos] == '/')
											{
												if (line.length() > pos + 1)
												{
													if (line[pos + 1] == '/')
													{
														// end of line comment.
														break;
													}
													else if (line[pos + 1] == '*')
													{
														// start of multiline comment.
														inMultilineComment = true;
														pos += 2;
													}
													else
													{
														// Invalid token start.
														EP_WARN("Graphics::CompileShaderSrc(): Invalid variant option name!  "
															"Token begins with a forward slash.  File: {}, Line: {}", 
															HN_ToStr(inclPathHN), lineNumber);
														pos = std::min(
															{
																line.find_first_of(" \t\r", pos),
																line.find("//", pos),
																line.find("/*")
															});
													}
												}
											}
											else
											{
												// TODO: Enforce alphanumeric variant option names

												// Valid token start.
												size_t subpos = line.find_first_of(" \t\r/", pos);
												if (subpos != std::string_view::npos)
												{
													if (line[subpos] == '/')
													{
														if (line.length() > subpos + 1)
														{
															if (line[subpos + 1] == '/')
															{
																// Valid token, terminated by end of line comment
																inclPragmaOptStrNameIndices[inclPathHN].back()
																	.emplace_back(inclPragmaOptStrPositions[inclPathHN].size());
																inclPragmaOptStrPositions[inclPathHN]
																	.emplace_back(charsToLineStart + pos);
																inclPragmaOptStrLengths[inclPathHN]
																	.emplace_back(subpos - pos);

																pos = subpos + 2;
															}
															else if (line[subpos + 1] == '*')
															{
																// Valid token, terminated by multiline comment
																inclPragmaOptStrNameIndices[inclPathHN].back()
																	.emplace_back(inclPragmaOptStrPositions[inclPathHN].size());
																inclPragmaOptStrPositions[inclPathHN]
																	.emplace_back(charsToLineStart + pos);
																inclPragmaOptStrLengths[inclPathHN]
																	.emplace_back(subpos - pos);

																pos = subpos + 2;
																inMultilineComment = true;
															}
															else
															{
																// Invalid token, contains forward slash.
																EP_WARN("Graphics::CompileShaderSrc(): Invalid variant option name!  "
																	"Token begins with a forward slash.  File: {}, Line: {}",
																	HN_ToStr(inclPathHN), lineNumber);
																pos = std::min(
																	{
																		line.find_first_of(" \t\r", pos),
																		line.find("//", pos),
																		line.find("/*")
																	});
															}
														}
													}
													else
													{
														// Valid token, terminated by whitespace
														inclPragmaOptStrNameIndices[inclPathHN].back()
															.emplace_back(inclPragmaOptStrPositions[inclPathHN].size());
														inclPragmaOptStrPositions[inclPathHN]
															.emplace_back(charsToLineStart + pos);
														inclPragmaOptStrLengths[inclPathHN]
															.emplace_back(subpos - pos);
														pos = subpos + 1;
													}
												}
												else
												{
													// Valid token, terminated by end of line.
													inclPragmaOptStrNameIndices[inclPathHN].back()
														.emplace_back(inclPragmaOptStrPositions[inclPathHN].size());
													inclPragmaOptStrPositions[inclPathHN]
														.emplace_back(charsToLineStart + pos);
													inclPragmaOptStrLengths[inclPathHN]
														.emplace_back(line.length() - pos);

													break;
												}
											}
										}
									}
									else // inMultilineComment
									{
										pos = line.find('*', pos);
										if (pos != std::string_view::npos)
										{
											pos++;
											if (line.length() > pos)
											{
												if (line[pos] == '/')
												{
													inMultilineComment = false;
													pos++;
												}
											}
										}
									}
								}

								inclSrcLengths[inclPathHN].emplace_back(charsToLineStart + line.length() + 1);
								inclSrcStrings[inclPathHN].emplace_back(defineStr);
								inclSrcLengths[inclPathHN].emplace_back(defineLength);
								inclPragmaOptStrInsPoints[inclPathHN].emplace_back(inclSrcStrings[inclPathHN].size());
								inclSrcStrings[inclPathHN].emplace_back(nullptr);
								inclSrcLengths[inclPathHN].emplace_back(0);
								inclSrcStrings[inclPathHN].emplace_back(
									inclSrcBuffers[inclPathHN].data() + charsToLineStart + line.length());
							}
						}
					}
				}
			}
			else
			{
				// Parse for multiline comments
				while (pos != std::string_view::npos)
				{
					if (!inMultilineComment)
					{
						pos = line.find_first_of('/', pos);
						if (pos != std::string_view::npos)
						{
							pos++;
							if (line.length() > pos)
							{
								if (line[pos] == '/')
								{
									// end of line comment.
									break;
								}
								else if (line[pos] == '*')
								{
									// start of multiline comment.
									inMultilineComment = true;
									pos++;
								}
							}
						}
					}
					else // inMultilineComment
					{
						pos = line.find('*', pos);
						if (pos != std::string_view::npos)
						{
							pos++;
							if (line.length() > pos)
							{
								if (line[pos] == '/')
								{
									inMultilineComment = false;
									pos++;
								}
							}
						}
					}
				}
			}
		}
	}

	// OpenGL interprets the "-1" to mean the string is null-terminated
	inclSrcLengths[inclPathHN].emplace_back(-1);
}

bool Graphics::CompileShaderSrc(const std::string& src)
{
	bool returnVal = true;
	std::string shaderName;

	// Buffers of string pointers and string lengths to pass to glShaderSource()
	std::vector<GLchar*>  oglVertShaderSrcStrings = { versionStr, defineStr, vertexdefineStr };
	std::vector<GLint>    oglVertShaderSrcLengths = { versionLength, defineLength, vertexdefineLength };
	std::vector<GLchar*>  oglFragShaderSrcStrings = { versionStr, defineStr, fragmentdefineStr };
	std::vector<GLint>    oglFragShaderSrcLengths = { versionLength, defineLength, fragmentdefineLength };

	// Pragma option string substitution
	std::vector<size_t> pragmaOptStrPositions;
	std::vector<int> pragmaOptStrLengths;
	// Ordered by "#pragma" line no., then option index
	std::vector<std::vector<size_t>> pragmaOptStrNameIndices;
	std::vector<GLchar*> pragmaOptStrNameBufferSources;
	std::vector<int> pragmaOptStrInsPoints;

	// Used to handle string length calculation upon encountering a shader section directive
	std::vector<char*>* currentShaderSrcStrings = nullptr;
	std::vector<int>* currentShaderSrcLengths = nullptr;

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
			std::min(src.find_first_of('\n', charsToLineStart), src.length()) - charsToLineStart);

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
									EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
										"Shader source does not begin with \"#epshader\".  "
										"Line: {}", lineNumber);
									return false;
								}
							}
							else
							{
								// First token is '/', the only token on the line
								EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
									"Shader source does not begin with \"#epshader\".  "
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
												// Either this is a "//" (in which case the shader name is missing)
												// or a non-comment '/' immediately following "#epshader".
												EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
													"\"#epshader\" is not followed by a program name.  "
													"Line: {}", lineNumber);
												return false;
											}
											// else "#epshader" is immediately followed by "/*", handled in parseState 1
										}
										else
										{
											// '/' immediately following #epshader, last character on the line
											EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
												"\"#epshader\" followed by a non-whitespace character.  "
												"Line: {}", lineNumber);
											return false;
										}
									}
									else if (line[cursor] != ' ' && line[cursor] != '\t')
									{
										// Non-whitespace immediately after "#epshader"
										EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
											"\"#epshader\" followed by a non-whitespace character.  "
											"Line: {}", lineNumber);
										return false;
									}
								}
								else
								{
									// "#epshader" is last token on line
									EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
										"\"#epshader\" is not followed by a program name.  "
										"Line: {}", lineNumber);
									return false;
								}
								parseState = 1;
							}
							else
							{
								// '#' is the start of an invalid token
								EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
									"Shader source does not begin with \"#epshader\".  "
									"Line: {}", lineNumber);
								return false;
							}
						}
						else
						{
							// Invalid token
							EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
								"Shader source does not begin with \"#epshader\".  "
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

			case 1: // Looking for shader name (starts with cursor immediately following "#epshader")
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
									goto startOfValidatedToken;
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
										EP_WARN("Graphics::CompileShaderSrc(): Shader program \"/\" was previously "
											"compiled from a different source location!  Check shader source files "
											"for name collisions.  First Path: {}, Second Path: {}", 
											shaderSourcePaths[HN("/")], currentShaderSourcePath);
									}
									return true;
								}

								cursor = std::string_view::npos;
								parseState = 2;

								// Mark the start of the global section in the shader source arrays
								oglVertShaderSrcStrings.emplace_back(
									(char*)src.data() + charsToLineStart + line.length() + 1);
								oglFragShaderSrcStrings.emplace_back(
									(char*)src.data() + charsToLineStart + line.length() + 1);
							}
						}
						else
						{
							// Start of valid token
						startOfValidatedToken: // Used to jump out of previous if block if program name starts with '/'.

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
														EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" "
															"was previously compiled from a different source location!  "
															"Check shader source files for name collisions.  "
															"First Path: {}, Second Path: {}",
															shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
													}
													return true;
												}

												cursor = std::string_view::npos;
												parseState = 2;

												// Mark the start of the global section in the shader source arrays
												oglVertShaderSrcStrings.emplace_back(
													(char*)src.data() + charsToLineStart + line.length() + 1);
												oglFragShaderSrcStrings.emplace_back(
													(char*)src.data() + charsToLineStart + line.length() + 1);
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
														EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" "
															"was previously compiled from a different source location!  "
															"Check shader source files for name collisions.  "
															"First Path: {}, Second Path: {}",
															shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
													}
													return true;
												}

												cursor = std::string_view::npos;
												parseState = 2;
												subcursor += 2;

												// Mark the start of the global section in the shader source arrays
												oglVertShaderSrcStrings.emplace_back(
													(char*)src.data() + charsToLineStart + line.length() + 1);
												oglFragShaderSrcStrings.emplace_back(
													(char*)src.data() + charsToLineStart + line.length() + 1);

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
																		EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
																			"after shader program name.  Token has been ignored.  "
																			"Program: {}, Line: {}", shaderName, lineNumber);
																		subcursor = 
																			std::min(
																				line.find("//", subcursor), 
																				line.find("/*", subcursor));
																	}
																}
																else
																{
																	// Not enough room for '/' to be start of a comment.
																	EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
																		"after shader program name.  Token has been ignored.  "
																		"Program: {}, Line: {}", shaderName, lineNumber);
																	break;
																}
															}
															else
															{
																// Unexpected token
																EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
																	"after shader program name.  Token has been ignored.  "
																	"Program: {}, Line: {}", shaderName, lineNumber);
																subcursor =
																	std::min(
																		line.find("//", subcursor),
																		line.find("/*", subcursor));
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
													EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" "
														"was previously compiled from a different source location!  "
														"Check shader source files for name collisions.  "
														"First Path: {}, Second Path: {}", 
														shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
												}
												return true;
											}

											cursor = std::string_view::npos;
											parseState = 2;

											// Mark the start of the global section in the shader source arrays
											oglVertShaderSrcStrings.emplace_back(
												(char*)src.data() + charsToLineStart + line.length() + 1);
											oglFragShaderSrcStrings.emplace_back(
												(char*)src.data() + charsToLineStart + line.length() + 1);
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
												EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" "
													"was previously compiled from a different source location!  "
													"Check shader source files for name collisions.  "
													"First Path: {}, Second Path: {}",
													shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
											}
											return true;
										}

										cursor = std::string_view::npos;
										parseState = 2;

										// Mark the start of the global section in the shader source arrays
										oglVertShaderSrcStrings.emplace_back(
											(char*)src.data() + charsToLineStart + line.length() + 1);
										oglFragShaderSrcStrings.emplace_back(
											(char*)src.data() + charsToLineStart + line.length() + 1);

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
																EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
																	"after shader program name.  Token has been ignored.  "
																	"Program: {}, Line: {}", shaderName, lineNumber);
																subcursor =
																	std::min(
																		line.find("//", subcursor),
																		line.find("/*", subcursor));
															}
														}
														else
														{
															// Not enough room for '/' to be start of a comment.
															EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
																"after shader program name.  Token has been ignored.  "
																"Program: {}, Line: {}", shaderName, lineNumber);
															break;
														}
													}
													else
													{
														// Unexpected token
														EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
															"after shader program name.  Token has been ignored.  "
															"Program: {}, Line: {}", shaderName, lineNumber);
														subcursor =
															std::min(
																line.find("//", subcursor),
																line.find("/*", subcursor));
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
											EP_WARN("Graphics::CompileShaderSrc(): Shader program \"{}\" "
												"was previously compiled from a different source location!  "
												"Check shader source files for name collisions.  "
												"First Path: {}, Second Path: {}",
												shaderName, shaderSourcePaths[hn], currentShaderSourcePath);
										}
										return true;
									}

									cursor = std::string_view::npos;
									parseState = 2;

									// Mark the start of the global section in the shader source arrays
									oglVertShaderSrcStrings.emplace_back(
										(char*)src.data() + charsToLineStart + line.length() + 1);
									oglFragShaderSrcStrings.emplace_back(
										(char*)src.data() + charsToLineStart + line.length() + 1);
								}
							}
						}
					}
					else
					{
						// No token after "#epshader"
						EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
							"\"#epshader\" is not followed by a program name.  "
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
							EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
								"\"#epshader\" is not followed by a program name.  "
								"Line: {}", lineNumber);
							return false;
						}
					}
					else
					{
						// No token after "#epshader"
						EP_ERROR("Graphics::CompileShaderSrc(): Shader compilation failed!  "
							"\"#epshader\" is not followed by a program name.  "
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
														pragmaOptStrNameIndices.emplace_back();
														pragmaOptStrNameBufferSources.push_back((GLchar*)src.data());
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
																EP_WARN("Graphics::CompileShaderSrc(): Invalid variant "
																	"option name!  Token begins with a forward slash.  "
																	"Program: {}, Line: {}", shaderName, lineNumber);
																cursor =
																	std::min(
																		{
																			line.find_first_of(" \t\r", cursor),
																			line.find("//", cursor),
																			line.find("/*")
																		});
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
																		pragmaOptStrNameIndices.back()
																			.emplace_back(pragmaOptStrPositions.size());
																		pragmaOptStrPositions
																			.emplace_back(charsToLineStart + cursor);
																		pragmaOptStrLengths
																			.emplace_back(subcursor - cursor);

																		cursor = subcursor + 2;
																	}
																	else if (line[subcursor + 1] == '*')
																	{
																		// Valid token, terminated by multiline comment
																		pragmaOptStrNameIndices.back()
																			.emplace_back(pragmaOptStrPositions.size());
																		pragmaOptStrPositions
																			.emplace_back(charsToLineStart + cursor);
																		pragmaOptStrLengths
																			.emplace_back(subcursor - cursor);

																		cursor = subcursor + 2;
																		inMultilineComment = true;
																	}
																	else
																	{
																		// Invalid token, contains forward slash.
																		EP_WARN("Graphics::CompileShaderSrc(): Invalid variant "
																			"option name!  Token contains a forward slash.  "
																			"Program: {}, Line: {}", shaderName, lineNumber);
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
																pragmaOptStrNameIndices.back()
																	.emplace_back(pragmaOptStrPositions.size());
																pragmaOptStrPositions
																	.emplace_back(charsToLineStart + cursor);
																pragmaOptStrLengths
																	.emplace_back(subcursor - cursor);
																cursor = subcursor + 1;
															}
														}
														else
														{
															// Valid token, terminated by end of line.
															pragmaOptStrNameIndices.back()
																.emplace_back(pragmaOptStrPositions.size());
															pragmaOptStrPositions
																.emplace_back(charsToLineStart + cursor);
															pragmaOptStrLengths
																.emplace_back(line.length() - cursor);

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
											oglVertShaderSrcLengths.emplace_back(
												charsToLineStart + line.length() + 1 - (oglVertShaderSrcStrings.back() - src.data()));
											oglFragShaderSrcLengths.emplace_back(
												charsToLineStart + line.length() + 1 - (oglFragShaderSrcStrings.back() - src.data()));

											// Close off multiline comment for insertion purposes
											if (inMultilineComment)
											{
												oglVertShaderSrcStrings.emplace_back(closeCommentStr);
												oglVertShaderSrcLengths.emplace_back(closeCommentLength);
												oglFragShaderSrcStrings.emplace_back(closeCommentStr);
												oglFragShaderSrcLengths.emplace_back(closeCommentLength);
											}

											// Add "#define "
											oglVertShaderSrcStrings.emplace_back(defineStr);
											oglVertShaderSrcLengths.emplace_back(defineLength);
											oglFragShaderSrcStrings.emplace_back(defineStr);
											oglFragShaderSrcLengths.emplace_back(defineLength);

											// Add the insertion point
											pragmaOptStrInsPoints.emplace_back(oglVertShaderSrcLengths.size());
											oglVertShaderSrcLengths.emplace_back(0);
											oglVertShaderSrcStrings.emplace_back(nullptr);
											oglFragShaderSrcLengths.emplace_back(0);
											oglFragShaderSrcStrings.emplace_back(nullptr);

											// Add newline
											oglVertShaderSrcStrings.emplace_back(newlineStr);
											oglVertShaderSrcLengths.emplace_back(newlineLength);
											oglFragShaderSrcStrings.emplace_back(newlineStr);
											oglFragShaderSrcLengths.emplace_back(newlineLength);

											// Reopen multiline comment
											if (inMultilineComment)
											{
												oglVertShaderSrcStrings.emplace_back(openCommentStr);
												oglVertShaderSrcLengths.emplace_back(openCommentLength);
												oglFragShaderSrcStrings.emplace_back(openCommentStr);
												oglFragShaderSrcLengths.emplace_back(openCommentLength);
											}

											// Start the new source string
											oglVertShaderSrcStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
											oglFragShaderSrcStrings.emplace_back((char*)src.data() + charsToLineStart + line.length() + 1);
										}
									}
								}
							}
							else if (line.compare(cursor, 8, "#include") == 0)
							{
								size_t charsToIncludeDirective = cursor + charsToLineStart;

								cursor = line.find_first_not_of(" \t\r", cursor + 8);
								if (cursor != std::string_view::npos)
								{
									if (line[cursor] == '<' || line[cursor] == '\"')
									{
										size_t subcursor =
											line.find_first_of(line[cursor] == '<' ? ">/" : "\"/", cursor + 1);
										for (; subcursor != std::string_view::npos; )
										{
											if (!inMultilineComment)
											{
												if (line[subcursor] == '>' || line[subcursor] == '"')
												{
													std::string inclPath;
													if (line[subcursor] == '>')
													{
														inclPath =
															File::engineShadersPath + 
															std::string(line.substr(cursor + 1, subcursor - cursor - 1));
													}
													else // line[subcursor] == '"'
													{
														inclPath =
															std::string(line.substr(cursor + 1, subcursor - cursor - 1));
													}

													HashName inclPathHash = HN(inclPath);
													if (!inclSrcBuffers.count(inclPathHash))
													{
														if (File::Exists(inclPath))
														{
															if (File::LoadTextFile(inclPath, &inclSrcBuffers[inclPathHash])
																== File::ErrorCode::Success)
															{
																parseIncludeFile(inclPathHash);
															}
															else
															{
																inclSrcBuffers.erase(inclPathHash);
																EP_WARN("Graphics::CompileShaderSource(): "
																	"Could not load include file \"{}\".  "
																	"Program: {}, Line: {}", inclPath, shaderName, lineNumber);
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
															EP_WARN("Graphics::CompileShaderSource(): "
																"Include file \"{}\" does not exist!  "
																"Program: {}, Line: {}", inclPath, shaderName, lineNumber);
															break;
														}
													}

													// Integrate the include file into the shader source buffers

													// Terminate the current source string
													oglVertShaderSrcLengths.emplace_back(
														charsToIncludeDirective - (oglVertShaderSrcStrings.back() - src.data()));
													oglFragShaderSrcLengths.emplace_back(
														charsToIncludeDirective - (oglFragShaderSrcStrings.back() - src.data()));

													// Close off any ongoing multiline comment
													if (inMultilineComment)
													{
														oglVertShaderSrcStrings.emplace_back(closeCommentStr);
														oglVertShaderSrcLengths.emplace_back(closeCommentLength);
														oglFragShaderSrcStrings.emplace_back(closeCommentStr);
														oglFragShaderSrcLengths.emplace_back(closeCommentLength);
													}

													// Pragma option insertion points
													pragmaOptStrInsPoints.insert(
														pragmaOptStrInsPoints.end(),
														inclPragmaOptStrInsPoints[inclPathHash].begin(),
														inclPragmaOptStrInsPoints[inclPathHash].end());
													auto rit = pragmaOptStrInsPoints.rbegin();
													for (unsigned int i = 0;
														i < inclPragmaOptStrInsPoints[inclPathHash].size();
														i++)
													{
														(*rit) += oglVertShaderSrcStrings.size();
													}
													pragmaOptStrPositions.insert(
														pragmaOptStrPositions.end(),
														inclPragmaOptStrPositions[inclPathHash].begin(),
														inclPragmaOptStrPositions[inclPathHash].end());
													pragmaOptStrLengths.insert(
														pragmaOptStrLengths.end(),
														inclPragmaOptStrLengths[inclPathHash].begin(),
														inclPragmaOptStrLengths[inclPathHash].end());
													pragmaOptStrNameIndices.insert(
														pragmaOptStrNameIndices.end(),
														inclPragmaOptStrNameIndices[inclPathHash].begin(),
														inclPragmaOptStrNameIndices[inclPathHash].end());
													for (unsigned int i = 0;
														i < inclPragmaOptStrNameIndices[inclPathHash].size();
														i++)
													{
														pragmaOptStrNameBufferSources
															.emplace_back(inclSrcBuffers[inclPathHash].data());
													}

													// Add contents of include file
													oglVertShaderSrcStrings.insert(
														oglVertShaderSrcStrings.end(),
														inclSrcStrings[inclPathHash].begin(),
														inclSrcStrings[inclPathHash].end());
													oglVertShaderSrcLengths.insert(
														oglVertShaderSrcLengths.end(),
														inclSrcLengths[inclPathHash].begin(),
														inclSrcLengths[inclPathHash].end());
													oglFragShaderSrcStrings.insert(
														oglFragShaderSrcStrings.end(),
														inclSrcStrings[inclPathHash].begin(),
														inclSrcStrings[inclPathHash].end());
													oglFragShaderSrcLengths.insert(
														oglFragShaderSrcLengths.end(),
														inclSrcLengths[inclPathHash].begin(),
														inclSrcLengths[inclPathHash].end());

													// Add newline
													oglVertShaderSrcStrings.emplace_back(newlineStr);
													oglVertShaderSrcLengths.emplace_back(newlineLength);
													oglFragShaderSrcStrings.emplace_back(newlineStr);
													oglFragShaderSrcLengths.emplace_back(newlineLength);

													// Reopen any ongoing multiline comment
													if (inMultilineComment)
													{
														oglVertShaderSrcStrings.emplace_back(openCommentStr);
														oglVertShaderSrcLengths.emplace_back(openCommentLength);
														oglFragShaderSrcStrings.emplace_back(openCommentStr);
														oglFragShaderSrcLengths.emplace_back(openCommentLength);
													}

													// Start the new source string
													oglVertShaderSrcStrings.emplace_back(
														(char*)src.data() + charsToLineStart + line.length() + 1);
													oglFragShaderSrcStrings.emplace_back(
														(char*)src.data() + charsToLineStart + line.length() + 1);

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
														EP_WARN("Graphics::CompileShaderSource(): \"#include\" "
															"followed by illegitimate string.  "
															"Program: {}, Line:{}", shaderName, lineNumber);
														break;
													}
													else // Slash is part of include path
													{
														subcursor = line.find_first_of(
															line[cursor] == '<' ? ">/" : "\"/", subcursor + 1);
													}
												}
												else
												{
													EP_WARN("Graphics::CompileShaderSource(): \"#include\" "
														"followed by illegitimate string.  "
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
														subcursor = line.find_first_of(
															line[cursor] == '<' ? ">/" : "\"/", subcursor + 2);
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
								oglVertShaderSrcLengths.emplace_back(
									charsToLineStart + cursor - (oglVertShaderSrcStrings.back() - src.data()));
								oglFragShaderSrcLengths.emplace_back(
									charsToLineStart + cursor - (oglFragShaderSrcStrings.back() - src.data()));

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
													EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
														"after \"#vertex\" directive.  Token has been ignored.  "
														"Program: {}, Line: {}", shaderName, lineNumber);
													cursor = line.find_first_of(" \t\r/", cursor);
												}
											}
											else
											{
												// Slash was the start of an invalid token, and last character on line
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
													"after \"#vertex\" directive.  Token has been ignored.  "
													"Program: {}, Line: {}", shaderName, lineNumber);
												cursor = std::string_view::npos;
											}
										}
										else
										{
											// unexpected
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
												"after \"#vertex\" directive.  Token has been ignored.  "
												"Program: {}, Line: {}", shaderName, lineNumber);
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
								oglVertShaderSrcStrings.emplace_back(
									(char*)src.data() + charsToLineStart + line.length() + 1);
								currentShaderSrcStrings = &oglVertShaderSrcStrings;
								currentShaderSrcLengths = &oglVertShaderSrcLengths;
								parseState = 3;
							}
							else if (line.compare(cursor, 9, "#fragment") == 0)
							{
								// Terminate the current source strings to just before "#fragment"
								oglVertShaderSrcLengths.emplace_back(
									charsToLineStart + cursor - (oglVertShaderSrcStrings.back() - src.data()));
								oglFragShaderSrcLengths.emplace_back(
									charsToLineStart + cursor - (oglFragShaderSrcStrings.back() - src.data()));

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
													EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
														"after \"#fragment\" directive.  Token has been ignored.  "
														"Program: {}, Line: {}", shaderName, lineNumber);
													cursor = line.find_first_of(" \t\r/", cursor);
												}
											}
											else
											{
												// Slash was the start of an invalid token, and last character on line
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
													"after \"#fragment\" directive.  Token has been ignored.  "
													"Program: {}, Line: {}", shaderName, lineNumber);
												cursor = std::string_view::npos;
											}
										}
										else
										{
											// unexpected
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
												"after \"#fragment\" directive.  Token has been ignored.  "
												"Program: {}, Line: {}", shaderName, lineNumber);
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
								oglFragShaderSrcStrings.emplace_back(
									(char*)src.data() + charsToLineStart + line.length() + 1);
								currentShaderSrcStrings = &oglFragShaderSrcStrings;
								currentShaderSrcLengths = &oglFragShaderSrcLengths;
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
							currentShaderSrcLengths->emplace_back(
								charsToLineStart + cursor - (oglVertShaderSrcStrings.back() - src.data()));

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
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
													"after \"#vertex\" directive.  Token has been ignored.  "
													"Program: {}, Line: {}", shaderName, lineNumber);
												cursor = line.find_first_of(" \t\r/", cursor);
											}
										}
										else
										{
											// Slash was the start of an invalid token, and last character on line
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
												"after \"#vertex\" directive.  Token has been ignored.  "
												"Program: {}, Line: {}", shaderName, lineNumber);
											cursor = std::string_view::npos;
										}
									}
									else
									{
										// unexpected
										EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
											"after \"#vertex\" directive.  Token has been ignored.  "
											"Program: {}, Line: {}", shaderName, lineNumber);
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
							oglVertShaderSrcStrings.emplace_back(
								(char*)src.data() + charsToLineStart + line.length() + 1);
							currentShaderSrcStrings = &oglVertShaderSrcStrings;
							currentShaderSrcLengths = &oglVertShaderSrcLengths;
							parseState = 3;
						}
						else if (line.compare(cursor, 9, "#fragment") == 0)
						{
							// Terminate the current source strings to just before "#fragment"
							currentShaderSrcLengths->emplace_back(
								charsToLineStart + cursor - (oglVertShaderSrcStrings.back() - src.data()));

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
												EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
													"after \"#fragment\" directive.  Token has been ignored.  "
													"Program: {}, Line: {}", shaderName, lineNumber);
												cursor = line.find_first_of(" \t\r/", cursor);
											}
										}
										else
										{
											// Slash was the start of an invalid token, and last character on line
											EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
												"after \"#fragment\" directive.  Token has been ignored.  "
												"Program: {}, Line: {}", shaderName, lineNumber);
											cursor = std::string_view::npos;
										}
									}
									else
									{
										// unexpected
										EP_WARN("Graphics::CompileShaderSrc(): Unexpected token "
											"after \"#fragment\" directive.  Token has been ignored.  "
											"Program: {}, Line: {}", shaderName, lineNumber);
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
							oglFragShaderSrcStrings.emplace_back(
								(char*)src.data() + charsToLineStart + line.length() + 1);
							currentShaderSrcStrings = &oglFragShaderSrcStrings;
							currentShaderSrcLengths = &oglFragShaderSrcLengths;
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

	EP_ASSERT(currentShaderSrcStrings);
	if (currentShaderSrcStrings->size() > currentShaderSrcLengths->size())
		currentShaderSrcLengths->emplace_back(-1); // OpenGL interprets the "-1" to mean the string is null-terminated


	// Variant compilation!

	// Calculate the number of variants
	int numOfVariants = 1;
	for (int i = 0; i < pragmaOptStrNameIndices.size(); i++)
	{
		numOfVariants *= pragmaOptStrNameIndices[i].size();
	}

	GLchar oglStringOutBuffer[1024];

	GLsizei oglUniformListsSize = 0;
	GLuint* oglUniformListIndices = nullptr; // Simple list of all integers between 0 and oglUniformListsSize.
	GLint* oglUniformBlockIndices = nullptr;
	GLint* oglUniformTypes = nullptr;
	GLint* oglUniformArraySizes = nullptr;

	// Optional preprocessed shader dump
#ifdef EP_DUMP_GLSL
	Enterprise::File::TextFileWriter outSrc;
	std::error_code ec;
	std::filesystem::remove_all(Enterprise::File::VirtualPathToNative("d/GLSL Dump/" + shaderName), ec);
#endif

	// Assemble and compile all shader variants
	for (int i = 0; i < numOfVariants; i++) // For every variant
	{
		size_t divvalue = 1;
		std::set<HashName> variantLookupKey;

		// Assemble shader variants
		for (int j = 0; j < pragmaOptStrNameIndices.size(); j++) // For each #pragma option statement
		{
			size_t modvalue = pragmaOptStrNameIndices[j].size();
			// j is the index of the pragma statement
			// (i / divvalue % modvalue) is the index for this variant's option

			// Selectively handle null option name ("_")
			GLchar* srcBuff = pragmaOptStrNameBufferSources[j];
			if (srcBuff[pragmaOptStrPositions[pragmaOptStrNameIndices[j][i / divvalue % modvalue]]] == '_'
				&& pragmaOptStrLengths[pragmaOptStrNameIndices[j][i / divvalue % modvalue]] == 1)
			{
				oglVertShaderSrcStrings[pragmaOptStrInsPoints[j] - (GLint)1] = spaceStr;
				oglVertShaderSrcLengths[pragmaOptStrInsPoints[j] - (GLint)1] = spaceLength;
				oglVertShaderSrcStrings[pragmaOptStrInsPoints[j]] = spaceStr;
				oglVertShaderSrcLengths[pragmaOptStrInsPoints[j]] = spaceLength;

				oglFragShaderSrcStrings[pragmaOptStrInsPoints[j] - (GLint)1] = spaceStr;
				oglFragShaderSrcLengths[pragmaOptStrInsPoints[j] - (GLint)1] = spaceLength;
				oglFragShaderSrcStrings[pragmaOptStrInsPoints[j]] = spaceStr;
				oglFragShaderSrcLengths[pragmaOptStrInsPoints[j]] = spaceLength;
			}
			else
			{
				oglVertShaderSrcStrings[pragmaOptStrInsPoints[j] - (GLint)1] = defineStr;
				oglVertShaderSrcLengths[pragmaOptStrInsPoints[j] - (GLint)1] = defineLength;
				oglVertShaderSrcStrings[pragmaOptStrInsPoints[j]] =
					srcBuff + pragmaOptStrPositions[pragmaOptStrNameIndices[j][i / divvalue % modvalue]];
				oglVertShaderSrcLengths[pragmaOptStrInsPoints[j]] =
					pragmaOptStrLengths[pragmaOptStrNameIndices[j][i / divvalue % modvalue]];

				oglFragShaderSrcStrings[pragmaOptStrInsPoints[j] - (GLint)1] = defineStr;
				oglFragShaderSrcLengths[pragmaOptStrInsPoints[j] - (GLint)1] = defineLength;
				oglFragShaderSrcStrings[pragmaOptStrInsPoints[j]] =
					oglVertShaderSrcStrings[pragmaOptStrInsPoints[j]];
				oglFragShaderSrcLengths[pragmaOptStrInsPoints[j]] =
					oglVertShaderSrcLengths[pragmaOptStrInsPoints[j]];

				auto keyinsertresult = variantLookupKey.emplace(
					HN(oglVertShaderSrcStrings[pragmaOptStrInsPoints[j]], oglVertShaderSrcLengths[pragmaOptStrInsPoints[j]]));

				if (!keyinsertresult.second)
				{
					EP_WARN("Graphics::CompileShaderSrc(): A shader option is defined twice in a single shader variant.  "
						"Program: {}, Option: {}", shaderName,
						std::string(oglVertShaderSrcStrings[pragmaOptStrInsPoints[j]],
							oglVertShaderSrcLengths[pragmaOptStrInsPoints[j]]));
				}
			}

			divvalue *= modvalue;
		}

		// Compilation and linking
		if (oglShaderPrograms[HN(shaderName)].count(variantLookupKey) == 0)
		{
			// Create shader objects
			EP_ASSERT_SLOW(oglVertShaderSrcStrings.size() == oglVertShaderSrcLengths.size());
			EP_ASSERT_SLOW(oglFragShaderSrcStrings.size() == oglFragShaderSrcLengths.size());

			// Upload source code
			GLuint vshader = EP_GL(glCreateShader(GL_VERTEX_SHADER));
			GLuint fshader = EP_GL(glCreateShader(GL_FRAGMENT_SHADER));
			EP_GL(glShaderSource(vshader,
				(GLsizei)oglVertShaderSrcStrings.size(),
				oglVertShaderSrcStrings.data(),
				oglVertShaderSrcLengths.data()));
			EP_GL(glShaderSource(fshader,
				(GLsizei)oglFragShaderSrcStrings.size(),
				oglFragShaderSrcStrings.data(),
				oglFragShaderSrcLengths.data()));

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
					for (int j = 0; j < oglVertShaderSrcStrings.size(); j++)
					{
						if (oglVertShaderSrcLengths[j] >= 0)
						{
							view = std::string_view(oglVertShaderSrcStrings[j], oglVertShaderSrcLengths[j]);
						}
						else
						{
							view = std::string_view(oglVertShaderSrcStrings[j]);
						}
						std::cout << view;
					}
				}

				EP_GL(glDeleteShader(vshader));
				if (oglShaderPrograms[HN(shaderName)].size() == 0)
					oglShaderPrograms.erase(HN(shaderName));
				EP_DEBUGBREAK();
				returnVal = false;
				continue;
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
					for (int j = 0; j < oglFragShaderSrcStrings.size(); j++)
					{
						if (oglFragShaderSrcLengths[j] >= 0)
						{
							view = std::string_view(oglFragShaderSrcStrings[j], oglFragShaderSrcLengths[j]);
						}
						else
						{
							view = std::string_view(oglFragShaderSrcStrings[j]);
						}
						std::cout << view;
					}
				}

				EP_GL(glDeleteShader(vshader));
				EP_GL(glDeleteShader(fshader));
				if (oglShaderPrograms[HN(shaderName)].size() == 0)
					oglShaderPrograms.erase(HN(shaderName));
				EP_DEBUGBREAK();
				returnVal = false;
				continue;
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
				if (oglShaderPrograms[HN(shaderName)].size() == 0)
					oglShaderPrograms.erase(HN(shaderName));
				EP_DEBUGBREAK();
				returnVal = false;
				continue;
			}

#ifdef EP_DUMP_GLSL
			{
				// Dump pre-processsed source
				outSrc.Open("d/GLSL Dump/" + shaderName + "/Variant V" + std::to_string(i + 1) + ".glsl");
				outSrc << shaderName << " (" << program << "), Vertex Variant " << i + 1 << ": \n\n";

				std::string_view view;
				for (int j = 0; j < oglVertShaderSrcStrings.size(); j++)
				{
					if (oglVertShaderSrcLengths[j] >= 0)
					{
						view = std::string_view(oglVertShaderSrcStrings[j], oglVertShaderSrcLengths[j]);
					}
					else
					{
						view = std::string_view(oglVertShaderSrcStrings[j]);
					}
					outSrc << view;
				}
				outSrc.Close();
				outSrc.Open("d/GLSL Dump/" + shaderName + "/Variant F" + std::to_string(i + 1) + ".glsl");
				outSrc << shaderName << " (" << program << "), Fragment Variant " << i + 1 << ": \n\n";

				for (int j = 0; j < oglFragShaderSrcStrings.size(); j++)
				{
					if (oglFragShaderSrcLengths[j] >= 0)
					{
						view = std::string_view(oglFragShaderSrcStrings[j], oglFragShaderSrcLengths[j]);
					}
					else
					{
						view = std::string_view(oglFragShaderSrcStrings[j]);
					}
					outSrc << view;
				}
				outSrc.Close();
			}
#endif

			oglShaderPrograms[HN(shaderName)][variantLookupKey] = program;

			// Introspection
			// TODO: Move this outside of the compilation loop to reduce implicit pipeline flushes
			GLint count, size;

			// Uniform buffers

			EP_GL(glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count));
			for (int i = 0; i < count; i++)
			{
				// Get block deets
				GLsizei length;
				EP_GL(glGetActiveUniformBlockName(program, (GLuint)i, 1024, &length, oglStringOutBuffer)); // Block name
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
				oglUniformArraySizes = (GLint*)alloca(count * sizeof(GLint));
				oglUniformListsSize = count;
			}

			EP_GL(glGetActiveUniformsiv(program, count, oglUniformListIndices, GL_UNIFORM_BLOCK_INDEX, oglUniformBlockIndices));
			EP_GL(glGetActiveUniformsiv(program, count, oglUniformListIndices, GL_UNIFORM_TYPE, oglUniformTypes));
			EP_GL(glGetActiveUniformsiv(program, count, oglUniformListIndices, GL_UNIFORM_SIZE, oglUniformArraySizes));
			for (unsigned int i = 0; i < count; i++)
			{
				GLsizei nameLength;
				EP_GL(glGetActiveUniformName(program, i, 1024, &nameLength, oglStringOutBuffer));

				if (oglUniformBlockIndices[i] == -1) // Not part of a block.
				{
					if ((oglUniformTypes[i] >= GL_SAMPLER_1D				&& oglUniformTypes[i] <= GL_SAMPLER_2D_SHADOW) ||
						(oglUniformTypes[i] >= GL_SAMPLER_1D_ARRAY			&& oglUniformTypes[i] <= GL_SAMPLER_CUBE_SHADOW) ||
						(oglUniformTypes[i] >= GL_INT_SAMPLER_1D			&& oglUniformTypes[i] <= GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) ||
						(oglUniformTypes[i] >= GL_SAMPLER_2D_RECT			&& oglUniformTypes[i] <= GL_UNSIGNED_INT_SAMPLER_BUFFER) ||
						(oglUniformTypes[i] >= GL_SAMPLER_2D_MULTISAMPLE	&& oglUniformTypes[i] <= GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY))
					{
						HashName uniformNameHash;
						if (oglUniformArraySizes[i] > 1) // Is array
						{
							uniformNameHash = HN(oglStringOutBuffer, nameLength - 3);

							for (unsigned int j = 0; j < oglUniformArraySizes[i]; j++)
							{
								snprintf(oglStringOutBuffer + nameLength - 3, 1024 - (nameLength - 3), "[%u]", j);
								GLint loc = EP_GL(glGetUniformLocation(program, oglStringOutBuffer));
								samplerUniformLocations[HN(shaderName)][variantLookupKey][uniformNameHash].push_back(loc);
							}
						}
						else // Not array
						{
							uniformNameHash = HN(oglStringOutBuffer, nameLength);
							GLint loc = EP_GL(glGetUniformLocation(program, oglStringOutBuffer));
							samplerUniformLocations[HN(shaderName)][variantLookupKey][uniformNameHash].push_back(loc);
						}
						samplerUniformTypes[HN(shaderName)][variantLookupKey][uniformNameHash] = oglUniformTypes[i];
					}
					else
					{
						EP_ERROR("Graphics::CompileShaderSrc(): Shader program contains non-sampler uniform!  "
							"Program: {}, Uniform: {}", shaderName, oglStringOutBuffer);
					}
				}
			}

			// Vertex attributes
			EP_GL(glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count));
			for (int i = 0; i < count; i++)
			{
				GLint length;
				GLenum type;
				glGetActiveAttrib(program, i, 1024, &length, &size, &type, oglStringOutBuffer);

				shaderAttribLocations[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, length)] =
					glGetAttribLocation(program, oglStringOutBuffer);
				shaderAttribArrayLengths[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, length)] = size;
				shaderAttribTypes[HN(shaderName)][variantLookupKey][HN(oglStringOutBuffer, length)] = 
					oglTypeToShaderDataType(type);
			}
		}
		else
		{
			// Equivalent program is already compiled.
			EP_WARN("Graphics::CompileShaderSrc(): \"#pragma option\" statements result in equivalent variants.  "
				"Duplicate compilation skipped.  Program: {}", shaderName);
		}
	}

	return returnVal;
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
		for (const auto& [options, variant] : oglShaderPrograms[program])
		{
			EP_GL(glDeleteProgram(variant));
		}
		oglShaderPrograms.erase(program);
		shaderAttribLocations.erase(program);
		shaderAttribArrayLengths.erase(program);
		shaderAttribTypes.erase(program);

		if (activeShaderName == program)
		{
			activeShaderName = HN("EPNULLSHADER");
			oglActiveProgram = oglShaderPrograms[HN("EPNULLSHADER")][{}];;
		}
	}
}


void Graphics::EnableShaderOption(HashName option, bool updateShader)
{
	if (!selectedShaderOptions.emplace(option).second)
		EP_WARN("Graphics::EnableShaderOption(): Shader option already enabled!  "
			"Option: {}", HN_ToStr(option));

	if (updateShader) BindShader(selectedShaderName);
}

void Graphics::DisableShaderOption(HashName option, bool updateShader)
{
	if (selectedShaderOptions.erase(option) == 0)
		EP_WARN("Graphics::DisableShaderOption(): Shader option not enabled!  "
			"Option: {}", HN_ToStr(option));
	
	if (updateShader) BindShader(selectedShaderName);
}

bool Graphics::isShaderViable(HashName shader)
{
	if (oglShaderPrograms.count(shader) > 0)
	{
		return oglShaderPrograms[shader].count(selectedShaderOptions) > 0;
	}
	else
	{
		EP_WARN("Graphics::isShaderViable(): No shader program exists with provided HashName.  "
			"Program: {}", HN_ToStr(shader));
		return false;
	}
}


void Graphics::SetFallbackShader(HashName shader, std::set<HashName> options)
{
	if (fallbackShaderName != HN_NULL)
	{
		EP_WARN("Graphics::SetFallbackShader(): A fallback shader is already set!  "
			"Old: {}, New: {}", fallbackShaderName, HN_ToStr(shader));
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
				"Program: {}, Options: {}", HN_ToStr(shader), options);
		}
	}
	else
	{
		EP_ERROR("Graphics::SetFallbackShader(): No shader program exists with the provided HashName.  "
			"Fallback shader remains unset.  Program: {}", HN_ToStr(shader));
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
	selectedShaderName = program;

	if (isShaderViable(program))
	{
		activeShaderName = program;
		activeShaderOptions = selectedShaderOptions;
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
			activeShaderOptions = fallbackShaderOptions;
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
			activeShaderOptions = {};
			EP_WARN("Graphics::BindShader(): Unable to bind shader program and no fallback program has been set.  "
				"Program: {}", HN_ToStr(program));
			EP_DEBUGBREAK();

			oglActiveProgram = oglShaderPrograms[HN("EPNULLSHADER")][{}];
			EP_GL(glUseProgram(oglActiveProgram));
		}
	}
}


Graphics::UniformBufferHandle Graphics::CreateUniformBuffer(HashName name, size_t size, void* data, bool dynamic)
{
	EP_ASSERTF(size > 0, "Graphics::CreateUniformBuffer(): 'size' cannot be 0");
	EP_ASSERTF(size < maxUniformBufferSize, "Graphics::CreateUniformBuffer(): "
		"'size' exceeds limit set by OpenGL implementation.");

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
	EP_ASSERTF(offset + count <= uniformBufferSizes[buffer], "Graphics::SetUniformBufferData(): "
		"Copied data would exceed buffer bounds.");

	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, buffer));
	EP_GL(glBufferSubData(GL_UNIFORM_BUFFER, offset, count, data));
	EP_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void Graphics::BindUniformBuffer(UniformBufferHandle buffer)
{
	EP_ASSERTF(uniformBufferSizes.count(buffer) != 0, "Graphics::SetUniformBufferData(): 'buffer' is not a valid handle.");
	EP_GL(glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBindingPoints[uniformBlockNames[buffer]], buffer));
}

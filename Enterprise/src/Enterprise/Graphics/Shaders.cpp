#include "EP_PCH.h"
#include "Graphics.h"
#include "Enterprise/File/File.h"

using Enterprise::Graphics;
using Enterprise::File;

Graphics::ProgramRef Graphics::_activeProgram;
std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, unsigned int>> Graphics::_shaderAttributeIndices;
uint64_t Graphics::_enabledAttributes = 0;

// The OpenGL location of a uniform in a shader.
static std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, int>> uniformLocations;
// The data type of a uniform in a shader. TODO: Strip from release builds.
static std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, Graphics::ShaderDataType>> uniformTypes;
// The number of elements in a uniform array.
static std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, int>> uniformNumOfElements;

// Helper function: convert a ShaderDataType to an OpenGL enum representing the type.
static Graphics::ShaderDataType glTypeToShaderDataType(GLenum glType)
{
	switch (glType)
	{
		case GL_FLOAT:
			return Graphics::ShaderDataType::Float;
			break;
		case GL_FLOAT_VEC2:
			return Graphics::ShaderDataType::Float2;
			break;
		case GL_FLOAT_VEC3:
			return Graphics::ShaderDataType::Float3;
			break;
		case GL_FLOAT_VEC4:
			return Graphics::ShaderDataType::Float4;
			break;
		case GL_INT:
			return Graphics::ShaderDataType::Int;
			break;
		case GL_INT_VEC2:
			return Graphics::ShaderDataType::Int2;
			break;
		case GL_INT_VEC3:
			return Graphics::ShaderDataType::Int3;
			break;
		case GL_INT_VEC4:
			return Graphics::ShaderDataType::Int4;
			break;
		case GL_SAMPLER_1D:
			return Graphics::ShaderDataType::Int;
			break;
		case GL_SAMPLER_2D:
			return Graphics::ShaderDataType::Int;
			break;
		case GL_SAMPLER_3D:
			return Graphics::ShaderDataType::Int;
			break;
		case GL_UNSIGNED_INT:
			return Graphics::ShaderDataType::UInt;
			break;
		case GL_UNSIGNED_INT_VEC2:
			return Graphics::ShaderDataType::UInt2;
			break;
		case GL_UNSIGNED_INT_VEC3:
			return Graphics::ShaderDataType::UInt3;
			break;
		case GL_UNSIGNED_INT_VEC4:
			return Graphics::ShaderDataType::UInt4;
			break;
		case GL_FLOAT_MAT3:
			return Graphics::ShaderDataType::Mat3;
			break;
		case GL_FLOAT_MAT4:
			return Graphics::ShaderDataType::Mat4;
			break;
		default:
			EP_ASSERT_NOENTRY();
			return Graphics::ShaderDataType::none;
			break;
	}
}

static size_t sizeofShaderDataType(Graphics::ShaderDataType type)
{
	switch (type)
	{
	case Graphics::ShaderDataType::Float:
		return sizeof(float);
		break;
	case Graphics::ShaderDataType::Float2:
		return sizeof(float);
		break;
	case Graphics::ShaderDataType::Float3:
		return sizeof(float);
		break;
	case Graphics::ShaderDataType::Float4:
		return sizeof(float);
		break;
	case Graphics::ShaderDataType::Int:
		return sizeof(int);
		break;
	case Graphics::ShaderDataType::Int2:
		return sizeof(int);
		break;
	case Graphics::ShaderDataType::Int3:
		return sizeof(int);
		break;
	case Graphics::ShaderDataType::Int4:
		return sizeof(int);
		break;
	case Graphics::ShaderDataType::UInt:
		return sizeof(unsigned int);
		break;
	case Graphics::ShaderDataType::UInt2:
		return sizeof(unsigned int);
		break;
	case Graphics::ShaderDataType::UInt3:
		return sizeof(unsigned int);
		break;
	case Graphics::ShaderDataType::UInt4:
		return sizeof(unsigned int);
		break;
	default:
		EP_ASSERT_NOENTRY();
		return 0;
		break;
	}
}

static int numOfShaderDataTypeArgs(Graphics::ShaderDataType type)
{
	switch (type)
	{
	case Graphics::ShaderDataType::Float:
		return 1;
		break;
	case Graphics::ShaderDataType::Float2:
		return 1;
		break;
	case Graphics::ShaderDataType::Float3:
		return 3;
		break;
	case Graphics::ShaderDataType::Float4:
		return 4;
		break;
	case Graphics::ShaderDataType::Int:
		return 1;
		break;
	case Graphics::ShaderDataType::Int2:
		return 2;
		break;
	case Graphics::ShaderDataType::Int3:
		return 3;
		break;
	case Graphics::ShaderDataType::Int4:
		return 4;
		break;
	case Graphics::ShaderDataType::UInt:
		return 1;
		break;
	case Graphics::ShaderDataType::UInt2:
		return 2;
		break;
	case Graphics::ShaderDataType::UInt3:
		return 3;
		break;
	case Graphics::ShaderDataType::UInt4:
		return 4;
		break;
	default:
		EP_ASSERT_NOENTRY();
		return 0;
		break;
	}
}


// Vertex shaders

Graphics::VShaderRef Graphics::LoadVertexShaderFromString(std::string src)
{
	const char* source = src.c_str();
	unsigned int shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// Check for compilation errors
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		EP_ERROR(" [OpenGL] Vertex shader compilation failure! {}", message);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

Graphics::VShaderRef Graphics::LoadVertexShader(std::string path)
{
	File::TextFileReader reader(path);
	if(reader.GetError() != File::ErrorCode::Success) { return 0; }

	std::string src;
	while (!reader.isEOF())
	{
		src.append(reader.ReadNextLine() + "\n");
	}

	EP_INFO("Graphics: Compiling vertex shader \"{}\":", path);

	VShaderRef shader = LoadVertexShaderFromString(src);
	if (shader) { EP_INFO("          Success!"); 	}
	else 		{ EP_ERROR("          FAILED"); 	}
	return shader;
}

void Graphics::DeleteVertexShader(Graphics::VShaderRef shader)
{
	glDeleteShader(shader);
	EP_INFO("Graphics: Deleted vertex shader {}.", shader);
}

// Pixel shaders

Graphics::PShaderRef Graphics::LoadPixelShaderFromString(std::string src)
{
	const char* source = src.c_str();
	unsigned int shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// Check for compilation errors
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		EP_ERROR(" [OpenGL] Pixel shader compilation failure! {}", message);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

Graphics::PShaderRef Graphics::LoadPixelShader(std::string path)
{
	File::TextFileReader reader(path);
	if(reader.GetError() != File::ErrorCode::Success) { return 0; }

	std::string src;
	while (!reader.isEOF())
	{
		src.append(reader.ReadNextLine() + "\n");
	}

	EP_INFO("Graphics: Compiling pixel shader \"{}\":", path);

	PShaderRef shader = LoadPixelShaderFromString(src);
	if (shader) { EP_INFO("          Success!"); 	}
	else 		{ EP_ERROR("          FAILED"); 	}
	return shader;
}

void Graphics::DeletePixelShader(Graphics::PShaderRef shader)
{
	glDeleteShader(shader);
	EP_INFO("Graphics: Deleted pixel shader {}.", shader);
}

// Shader programs

Graphics::ProgramRef Graphics::LinkShaders(Graphics::VShaderRef vShader, Graphics::PShaderRef pShader)
{
	EP_INFO("Graphics: Linking shader program. V: {}, P: {}", vShader, pShader);

	if (!vShader && !pShader)
	{
		EP_ERROR("Graphics: Linking failed due to shader compilation failure.");
		return 0;
	}

	unsigned int program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, pShader);
	glLinkProgram(program);
	glDetachShader(program, vShader);
	glDetachShader(program, pShader);

	// Check for linking errors
	int result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(program, length, &length, message);
		EP_ERROR(" [OpenGL] Shader program linking error! {}", message);
		glDeleteProgram(program);
		return 0;
	}
	EP_INFO("          Success!");

	// Get the uniforms and vertex attributes used by the program
	{
		int maxNameLength;
		int count;
		int size;
		GLenum gltype;

		// Uniforms
		{
			glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
			char* uniformName = (char*)alloca(maxNameLength * sizeof(char));

			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
			for (int i = 0; i < count; i++)
			{
				glGetActiveUniform(program, (GLuint)i, maxNameLength, NULL, &size, &gltype, uniformName);

				// Array types have "[0]" appended to the end of the name, which need to be removed.
				for (int i = 0; uniformName[i] != '\0'; i++)
				{
					if (uniformName[i] == '[')
					{
						uniformName[i] = '\0';
						break;
					}
				}

				uniformLocations[program][HN(uniformName)] = glGetUniformLocation(program, uniformName);
				uniformTypes[program][HN(uniformName)] = glTypeToShaderDataType(gltype);
				uniformNumOfElements[program][HN(uniformName)] = size; // TODO: Use size to bounds-check Graphics::SetUniformArray().
			}
		}

		// Attributes
		{
			std::vector<std::tuple<HashName, unsigned int, ShaderDataType, uint64_t>> quadvertexinfo_sortable; // name, index, type, offset
			bool has_ep_pos = false, has_ep_uv = false, has_ep_tex = false;

			glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
			char* attributeName = (char*)alloca(maxNameLength * sizeof(char));

			glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
			for (int i = 0; i < count; i++)
			{
				glGetActiveAttrib(program, (GLuint)i, maxNameLength, NULL, &size, &gltype, attributeName);
				_shaderAttributeIndices[program][HN(attributeName)] = glGetAttribLocation(program, attributeName);

				if (HN(attributeName) == HN("ep_pos")) { has_ep_pos = true; }
				if (HN(attributeName) == HN("ep_uv")) { has_ep_uv = true; }
				if (HN(attributeName) == HN("ep_tex")) { has_ep_tex = true; }

				quadvertexinfo_sortable.push_back({ HN(attributeName),
												  _shaderAttributeIndices[program][HN(attributeName)],
												  glTypeToShaderDataType(gltype),
												  0 });
			}

			EP_ASSERT_SLOW(has_ep_pos && has_ep_uv && has_ep_tex);

			// Strictly order ep_pos, ep_uv, and ep_tex
			for (auto it = quadvertexinfo_sortable.begin();
				 it != quadvertexinfo_sortable.end();
				 ++it)
			{
				switch (std::get<0>(*it))
				{
				case "ep_pos"_HN:
					if (it != quadvertexinfo_sortable.begin())
					{
						std::iter_swap(it, quadvertexinfo_sortable.begin());
					}
					break;
				case "ep_uv"_HN:
					if (it != quadvertexinfo_sortable.begin() + 1)
					{
						std::iter_swap(it, quadvertexinfo_sortable.begin() + 1);
					}
					break;
				case "ep_tex"_HN:
					if (it != quadvertexinfo_sortable.begin() + 2)
					{
						std::iter_swap(it, quadvertexinfo_sortable.begin() + 2);
					}
					break;
				}
			}
			// Sort remaining items by size
			std::sort(quadvertexinfo_sortable.begin() + 3, quadvertexinfo_sortable.end(),
					  [](const std::tuple<HashName, unsigned int, ShaderDataType, uint64_t>& a,
						 const std::tuple<HashName, unsigned int, ShaderDataType, uint64_t>& b) -> bool
					  {
						  return sizeofShaderDataType(std::get<2>(a)) < sizeofShaderDataType(std::get<2>(b));
					  });

			// Set info for default attributes
			_quadBatchVertexInfo[program][HN("ep_pos")] =
			{
				std::get<1>(quadvertexinfo_sortable.at(0)),
				std::get<2>(quadvertexinfo_sortable.at(0)),
				offsetof(struct QuadBatchDefaultVertex, ep_pos)
			};
			_quadBatchVertexInfo[program][HN("ep_uv")] =
			{
				std::get<1>(quadvertexinfo_sortable.at(1)),
				std::get<2>(quadvertexinfo_sortable.at(1)),
				offsetof(struct QuadBatchDefaultVertex, ep_uv)
			};
			_quadBatchVertexInfo[program][HN("ep_tex")] =
			{
				std::get<1>(quadvertexinfo_sortable.at(2)),
				std::get<2>(quadvertexinfo_sortable.at(2)),
				offsetof(struct QuadBatchDefaultVertex, ep_tex)
			};

			// Calculate offsets for non-default attributes and overall vertex stride
			unsigned int stride = sizeof(QuadBatchDefaultVertex);
			for (auto it = quadvertexinfo_sortable.begin() + 3;
				 it != quadvertexinfo_sortable.end();
				 ++it)
			{
				auto [name, index, type, offset] = *it;
				_quadBatchVertexInfo[program][name] = { index, type, stride };
				stride += (sizeofShaderDataType(type) - stride % sizeofShaderDataType(type)) % sizeofShaderDataType(type); // padding
				stride += sizeofShaderDataType(type) * numOfShaderDataTypeArgs(type);
			}
			stride += (sizeof(float) - stride % sizeof(float)) % sizeof(float); // back-of-struct padding
			_quadBatchVertexStrides[program] = stride;
		}
	}

	return program;
}

void Graphics::BindProgram(ProgramRef program)
{
	EP_ASSERT_SLOW(program);
	if (_activeProgram != program)
	{
		_activeProgram = program;
		glUseProgram(program);
	}
}

void Graphics::DeleteProgram(ProgramRef program)
{
	EP_ASSERT(program);
	EP_ASSERTF(uniformLocations.count(program),
			   "Graphics: Attempted to delete a shader program with an invalid reference.");
	EP_ASSERTF(uniformTypes.count(program),
			   "Graphics: Attempted to delete a shader program with an invalid reference.");

	glDeleteProgram(program);

	uniformLocations.at(program).clear();
	uniformTypes.at(program).clear();
}


// Uniforms

void Graphics::SetTransform(Math::Mat4 transform)
{
	SetUniform(HN("transform"), transform);
}

void Graphics::SetProjection(Math::Mat4 projection)
{
	SetUniform(HN("projection"), projection);
}


void Graphics::SetUniform(HashName uniform, float value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform1f(uniformLocations[_activeProgram][uniform], value);
}

void Graphics::SetUniform(HashName uniform, float value1, float value2)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float2,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform2f(uniformLocations[_activeProgram][uniform], value1, value2);
}

void Graphics::SetUniform(HashName uniform, float value1, float value2, float value3)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float3,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform3f(uniformLocations[_activeProgram][uniform], value1, value2, value3);
}

void Graphics::SetUniform(HashName uniform, float value1, float value2, float value3, float value4)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float4,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform4f(uniformLocations[_activeProgram][uniform], value1, value2, value3, value4);
}

void Graphics::SetUniform(HashName uniform, Enterprise::Math::Vec2 value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float2,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform2f(uniformLocations[_activeProgram][uniform], value.x, value.y);
}

void Graphics::SetUniform(HashName uniform, Enterprise::Math::Vec3 value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float3,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform3f(uniformLocations[_activeProgram][uniform], value.x, value.y, value.z);
}

void Graphics::SetUniform(HashName uniform, Enterprise::Math::Vec4 value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Float4,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform4f(uniformLocations[_activeProgram][uniform], value.x, value.y, value.z, value.w);
}

void Graphics::SetUniform(HashName uniform, int value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Int,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform1i(uniformLocations[_activeProgram][uniform], value);
}

void Graphics::SetUniform(HashName uniform, int value1, int value2)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Int2,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform2i(uniformLocations[_activeProgram][uniform], value1, value2);
}

void Graphics::SetUniform(HashName uniform, int value1, int value2, int value3)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Int3,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform3i(uniformLocations[_activeProgram][uniform], value1, value2, value3);
}

void Graphics::SetUniform(HashName uniform, int value1, int value2, int value3, int value4)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Int4,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform4i(uniformLocations[_activeProgram][uniform], value1, value2, value3, value4);
}

void Graphics::SetUniform(HashName uniform, unsigned int value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::UInt,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform1ui(uniformLocations[_activeProgram][uniform], value);
}

void Graphics::SetUniform(HashName uniform, unsigned int value1, unsigned int value2)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::UInt2,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform2ui(uniformLocations[_activeProgram][uniform], value1, value2);
}

void Graphics::SetUniform(HashName uniform, unsigned int value1, unsigned int value2, unsigned int value3)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::UInt3,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform3ui(uniformLocations[_activeProgram][uniform], value1, value2, value3);
}

void Graphics::SetUniform(HashName uniform, unsigned int value1, unsigned int value2, unsigned int value3, unsigned int value4)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::UInt4,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniform4ui(uniformLocations[_activeProgram][uniform], value1, value2, value3, value4);
}

void Graphics::SetUniform(HashName uniform, Enterprise::Math::Mat3 value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Mat3,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniformMatrix3fv(uniformLocations[_activeProgram][uniform], 1, GL_TRUE, (GLfloat*)&value);
}

void Graphics::SetUniform(HashName uniform, Enterprise::Math::Mat4 value)
{
	EP_ASSERTF_SLOW(uniformLocations[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram].count(uniform),
					"Graphics: Attempted to set uniform not present in the active shader program.");
	EP_ASSERTF_SLOW(uniformTypes[_activeProgram][uniform] == ShaderDataType::Mat4,
					"Graphics: Attempted to set uniform with wrong data type.");

	glUniformMatrix4fv(uniformLocations[_activeProgram][uniform], 1, GL_TRUE, (GLfloat*)&value);
}

void Graphics::SetUniformArray(HashName uniform, unsigned int count, Graphics::ShaderDataType type, void* src)
{
	EP_ASSERT_SLOW(uniformLocations[_activeProgram].count(uniform));

	switch (type)
	{
	case ShaderDataType::Float:
		glUniform1fv(uniformLocations[_activeProgram][uniform], count, (GLfloat*)src);
		break;
	case ShaderDataType::Float2:
		glUniform2fv(uniformLocations[_activeProgram][uniform], count, (GLfloat*)src);
		break;
	case ShaderDataType::Float3:
		glUniform3fv(uniformLocations[_activeProgram][uniform], count, (GLfloat*)src);
		break;
	case ShaderDataType::Float4:
		glUniform4fv(uniformLocations[_activeProgram][uniform], count, (GLfloat*)src);
		break;
	case ShaderDataType::Int:
		glUniform1iv(uniformLocations[_activeProgram][uniform], count, (GLint*)src);
		break;
	case ShaderDataType::Int2:
		glUniform2iv(uniformLocations[_activeProgram][uniform], count, (GLint*)src);
		break;
	case ShaderDataType::Int3:
		glUniform3iv(uniformLocations[_activeProgram][uniform], count, (GLint*)src);
		break;
	case ShaderDataType::Int4:
		glUniform4iv(uniformLocations[_activeProgram][uniform], count, (GLint*)src);
		break;
	case ShaderDataType::UInt:
		glUniform1uiv(uniformLocations[_activeProgram][uniform], count, (GLuint*)src);
		break;
	case ShaderDataType::UInt2:
		glUniform2uiv(uniformLocations[_activeProgram][uniform], count, (GLuint*)src);
		break;
	case ShaderDataType::UInt3:
		glUniform3uiv(uniformLocations[_activeProgram][uniform], count, (GLuint*)src);
		break;
	case ShaderDataType::UInt4:
		glUniform4uiv(uniformLocations[_activeProgram][uniform], count, (GLuint*)src);
		break;
	case ShaderDataType::Mat3:
		glUniformMatrix3fv(uniformLocations[_activeProgram][uniform], count, GL_TRUE, (GLfloat*)src);
		break;
	case ShaderDataType::Mat4:
		glUniformMatrix4fv(uniformLocations[_activeProgram][uniform], count, GL_TRUE, (GLfloat*)src);
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}

#include "EP_PCH.h"
#include "Graphics.h"
#include "Enterprise/File/File.h"

using Enterprise::Graphics;
using Enterprise::File;

Graphics::ProgramRef Graphics::_activeProgram;
std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, unsigned int>> Graphics::_shaderAttributeIndices;

// The OpenGL location of a uniform in a shader.
static std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, int>> uniformLocations;
// The data type of a uniform in a shader. TODO: Strip from relesae builds.
static std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, Graphics::ShaderDataType>> uniformTypes;

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
		GLenum type;

		// Uniforms
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
		char* uniformName = (char*)alloca(maxNameLength * sizeof(char));

		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
		for (int i = 0; i < count; i++)
		{
			glGetActiveUniform(program, (GLuint)i, maxNameLength, NULL, &size, &type, uniformName);
			uniformLocations[program][HN(uniformName)] = glGetUniformLocation(program, uniformName);
			uniformTypes[program][HN(uniformName)] = glTypeToShaderDataType(type);
		}

		// Attributes
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
		char* attributeName = (char*)alloca(maxNameLength * sizeof(char));

		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
		for (int i = 0; i < count; i++)
		{
			glGetActiveAttrib(program, (GLuint)i, maxNameLength, NULL, &size, &type, attributeName);
			_shaderAttributeIndices[program][HN(attributeName)] = glGetAttribLocation(program, attributeName);
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

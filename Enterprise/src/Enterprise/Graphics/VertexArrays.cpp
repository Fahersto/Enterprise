#include "EP_PCH.h"
#include "Graphics.h"

#include "OpenGLHelpers.h"
using Enterprise::Graphics;

static Graphics::VertexArrayHandle nextVAH = 1;
static Graphics::VertexArrayHandle boundArray = 0;

static std::map<Graphics::VertexArrayHandle, GLuint> vboHandles;
static std::map<Graphics::VertexArrayHandle, size_t> vboNumOfVertices;
static std::map<Graphics::VertexArrayHandle, size_t> vboVertexStrides;
static std::map<Graphics::VertexArrayHandle, std::vector<HashName>> vboAttributeNames;
static std::map<Graphics::VertexArrayHandle, std::map<HashName, size_t>> vboAttributeOffsets;
static std::map<Graphics::VertexArrayHandle, std::map<HashName, GLenum>> vboAttributeGLBaseTypes;
static std::map<Graphics::VertexArrayHandle, std::map<HashName, GLint>> vboAttributeGLTypeNumOfComponents;
static std::map<Graphics::VertexArrayHandle, std::map<HashName, size_t>> vboAttributeArrayLengths;

static std::map<Graphics::VertexArrayHandle, GLuint> iboHandles;
static std::map<Graphics::VertexArrayHandle, size_t> iboNumOfIndices;

extern GLuint oglActiveProgram; // Defined in Shaders.cpp


static size_t sizeofShaderDataType(Graphics::ShaderDataType type)
{
	switch (type)
	{
		case Enterprise::Graphics::ShaderDataType::none:
			return 0;
			break;
		case Enterprise::Graphics::ShaderDataType::Float:
			return sizeof(float);
			break;
		case Enterprise::Graphics::ShaderDataType::Vec2:
			return sizeof(glm::vec2);
			break;
		case Enterprise::Graphics::ShaderDataType::Vec3:
			return sizeof(glm::vec3);
			break;
		case Enterprise::Graphics::ShaderDataType::Vec4:
			return sizeof(glm::vec4);
			break;
		case Enterprise::Graphics::ShaderDataType::Int:
			return sizeof(int);
			break;
		case Enterprise::Graphics::ShaderDataType::UInt:
			return sizeof(unsigned int);
			break;
		case Enterprise::Graphics::ShaderDataType::Mat3:
			return sizeof(glm::mat3);
			break;
		case Enterprise::Graphics::ShaderDataType::Mat4:
			return sizeof(glm::mat4);
			break;
		default:
			EP_ASSERT_NOENTRY();
			return 0;
			break;
	}
}
static size_t alignofShaderDataType(Graphics::ShaderDataType type)
{
	switch (type)
	{
		case Enterprise::Graphics::ShaderDataType::none:
			return 0;
			break;
		case Enterprise::Graphics::ShaderDataType::Float:
			return alignof(float);
			break;
		case Enterprise::Graphics::ShaderDataType::Vec2:
			return alignof(glm::vec2);
			break;
		case Enterprise::Graphics::ShaderDataType::Vec3:
			return alignof(glm::vec3);
			break;
		case Enterprise::Graphics::ShaderDataType::Vec4:
			return alignof(glm::vec4);
			break;
		case Enterprise::Graphics::ShaderDataType::Int:
			return alignof(int);
			break;
		case Enterprise::Graphics::ShaderDataType::UInt:
			return alignof(unsigned int);
			break;
		case Enterprise::Graphics::ShaderDataType::Mat3:
			return alignof(glm::mat3);
			break;
		case Enterprise::Graphics::ShaderDataType::Mat4:
			return alignof(glm::mat4);
			break;
		default:
			EP_ASSERT_NOENTRY();
			return 0;
			break;
	}
}
static GLenum oglBaseTypeOfShaderDataType(Graphics::ShaderDataType type)
{
	switch (type)
	{
		case Enterprise::Graphics::ShaderDataType::Float:
			return GL_FLOAT;
			break;
		case Enterprise::Graphics::ShaderDataType::Vec2:
			return GL_FLOAT;
			break;
		case Enterprise::Graphics::ShaderDataType::Vec3:
			return GL_FLOAT;
			break;
		case Enterprise::Graphics::ShaderDataType::Vec4:
			return GL_FLOAT;
			break;
		case Enterprise::Graphics::ShaderDataType::Int:
			return GL_INT;
			break;
		case Enterprise::Graphics::ShaderDataType::UInt:
			return GL_UNSIGNED_INT;
			break;
		case Enterprise::Graphics::ShaderDataType::Mat3:
			return GL_FLOAT;
			break;
		case Enterprise::Graphics::ShaderDataType::Mat4:
			return GL_FLOAT;
			break;
		default:
			EP_ASSERT_NOENTRY();
			return 0;
			break;
	}
}
static GLint oglNumOfComponentsInShaderDataType(Graphics::ShaderDataType type)
{
	switch (type)
	{
		case Enterprise::Graphics::ShaderDataType::Float:
			return 1;
			break;
		case Enterprise::Graphics::ShaderDataType::Vec2:
			return 2;
			break;
		case Enterprise::Graphics::ShaderDataType::Vec3:
			return 3;
			break;
		case Enterprise::Graphics::ShaderDataType::Vec4:
			return 4;
			break;
		case Enterprise::Graphics::ShaderDataType::Int:
			return 1;
			break;
		case Enterprise::Graphics::ShaderDataType::UInt:
			return 1;
			break;
		default:
			EP_ASSERT_NOENTRY();
			return 0;
			break;
	}
}


Graphics::VertexArrayHandle Graphics::CreateVertexArray(size_t maxVertices, size_t maxIndices,
														size_t vertexStride,
														std::initializer_list<std::tuple<HashName, ShaderDataType, size_t, size_t>> vertexLayout,
														void* vertexData, void* indexData,
														bool dynamicVertices, bool dynamicIndices)
{
	EP_ASSERT(maxVertices != 0);
	EP_ASSERT(vertexLayout.size() > 0);

	EP_ASSERTF(nextVAH != 0, "Graphics::CreateVertexArray(): Exhausted vertex array handles!");
	boundArray = nextVAH;
	nextVAH++;

	size_t largestAlignment = 1;
	for (const auto& tup : vertexLayout) // 0: Attribute HashName 1: ShaderDataType 2: Array size 3: Attribute offset
	{
		if (std::get<3>(tup) % 4 != 0)
		{
			// Khronos says all attributes should be aligned to at least four bytes.
			// Source: https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Attribute_sizes
			EP_WARN("Graphics::CreateVertexArray(): Vertex attribute '{}' is not aligned to at least four bytes.",
					HN_ToStr(std::get<0>(tup)));
		}
		largestAlignment = std::max(largestAlignment, alignofShaderDataType(std::get<1>(tup)));

		vboAttributeNames[boundArray].push_back(std::get<0>(tup));
		vboAttributeOffsets[boundArray][std::get<0>(tup)] = std::get<3>(tup);
		vboAttributeGLBaseTypes[boundArray][std::get<0>(tup)] = oglBaseTypeOfShaderDataType(std::get<1>(tup));
		vboAttributeGLTypeNumOfComponents[boundArray][std::get<0>(tup)] = oglNumOfComponentsInShaderDataType(std::get<1>(tup));
		vboAttributeArrayLengths[boundArray][std::get<0>(tup)] = std::get<2>(tup);
	}

	vboVertexStrides[boundArray] = vertexStride;
	vboNumOfVertices[boundArray] = maxVertices;
	iboNumOfIndices[boundArray] = maxIndices;
	if (vertexStride % largestAlignment != 0)
	{
		EP_WARN("Graphics::CreateVertexArray(): Vertex stride does not share alignment with the largest vertex attribute.");
	}

	// Create VBO
	EP_GL(glGenBuffers(1, &vboHandles[boundArray]));
	EP_GL(glBindBuffer(GL_ARRAY_BUFFER, vboHandles[boundArray]));
	EP_GL(glBufferData(GL_ARRAY_BUFFER,
					   vertexStride * maxVertices,
					   vertexData,
					   dynamicVertices ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	// Create IBO
	if (maxIndices != 0)
	{
		EP_GL(glGenBuffers(1, &iboHandles[boundArray]));
		EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandles[boundArray]));
		EP_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
						   sizeof(GLuint) * maxIndices,
						   indexData,
						   dynamicIndices ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	}
	else
	{
		EP_ASSERTF(indexData == nullptr, "Graphics::CreateVertexArray(): "
				   "Non-null pointer to index data provided when 'maxIndices' equals '0'.");
		EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	return boundArray;
}

void Graphics::DeleteVertexArray(Graphics::VertexArrayHandle array)
{
	EP_GL(glDeleteBuffers(1, &vboHandles[array]));
	if (iboHandles.count(array) > 0)
	{
		EP_GL(glDeleteBuffers(1, &iboHandles[array]));
	}
	if (boundArray == array)
	{
		boundArray = 0;
	}

	vboHandles.erase(array);
	vboNumOfVertices.erase(array);
	vboVertexStrides.erase(array);
	vboAttributeNames.erase(array);
	vboAttributeOffsets.erase(array);
	vboAttributeGLBaseTypes.erase(array);
	vboAttributeGLTypeNumOfComponents.erase(array);
	vboAttributeArrayLengths.erase(array);

	iboHandles.erase(array);
	iboNumOfIndices.erase(array);
}

void Graphics::SetVertexData(Graphics::VertexArrayHandle array, void* data, size_t first, size_t count)
{
	EP_ASSERT_SLOW(array != 0);
	EP_ASSERT_SLOW(data != nullptr);
	EP_ASSERT_SLOW(count > 0);

	if (array != boundArray)
	{
		EP_GL(glBindBuffer(GL_ARRAY_BUFFER, vboHandles[array]));
		if (iboHandles.count(array) != 0)
		{
			EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandles[array]));
		}
		else
		{
			EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}
		boundArray = array;
	}
	EP_GL(glBufferSubData(GL_ARRAY_BUFFER, vboVertexStrides[array] * first, vboVertexStrides[array] * count, data));
}

void Graphics::SetIndexData(Graphics::VertexArrayHandle array, void* src, size_t first, size_t count)
{
	EP_ASSERT_SLOW(src != nullptr);
	EP_ASSERT_SLOW(count != 0);
	EP_ASSERT_SLOW(iboHandles.count(array) != 0);

	if (array != boundArray)
	{
		EP_GL(glBindBuffer(GL_ARRAY_BUFFER, vboHandles[array]));
		EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandles[array]));
		boundArray = array;
	}
	EP_GL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * first, sizeof(GLuint) * count, src));
}

void Graphics::BindVertexArrayForDraw(Graphics::VertexArrayHandle& array, size_t& count, size_t& first)
{
	// TODO: Check the size of the currently bound uniform buffers against their respective blocks in the program.
	// TODO: Skip everything if VA / Shader combination hasn't changed.

	bool arrayHasIBO = iboHandles.count(array) != 0;

	if (count == 0)
	{
		if (arrayHasIBO)
			count = iboNumOfIndices[array] - first;
		else
			count = vboNumOfVertices[array] - first;
	}

	if (array != boundArray)
	{
		EP_GL(glBindBuffer(GL_ARRAY_BUFFER, vboHandles[array]));
		if (arrayHasIBO)
		{
			EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandles[array]));
		}
		else
		{
			EP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}
		boundArray = array;
	}

	// Enable attribute locations and set pointers
	static uint32_t prevEnabledAttributesBF = 0;
	uint32_t enabledAttributesBF = 0;
	for (HashName attribute : vboAttributeNames[array])
	{
		if (shaderAttributeLocations[activeShaderName][activeShaderOptions].count(attribute) > 0)
		{
			EP_ASSERTF_SLOW(vboAttributeArrayLengths[array][attribute] == shaderAttributeArrayLengths[activeShaderName][activeShaderOptions][attribute],
							"Graphics::DrawTriangles(): Vertex attribute array differs in length between vertex array and bound shader program.");

			unsigned int attributeLocation = shaderAttributeLocations[activeShaderName][activeShaderOptions][attribute];
			for (int i = 0; i < shaderAttributeArrayLengths[activeShaderName][activeShaderOptions][attribute]; i++)
			{
				// Enable the vertex attribute index, if it's not already enabled
				if ((prevEnabledAttributesBF & BIT(attributeLocation + i)) == 0)
				{
					EP_GL(glEnableVertexAttribArray(attributeLocation + i));
				}
				enabledAttributesBF |= BIT(attributeLocation + i);

				// Set the attribute pointer
				GLenum glBaseType = vboAttributeGLBaseTypes[array][attribute];
				if (glBaseType == GL_FLOAT ||
					glBaseType == GL_DOUBLE ||
					glBaseType == GL_HALF_FLOAT ||
					glBaseType == GL_FIXED ||
					glBaseType == GL_INT_2_10_10_10_REV ||
					glBaseType == GL_UNSIGNED_INT_2_10_10_10_REV ||
					glBaseType == GL_UNSIGNED_INT_10F_11F_11F_REV)
				{
					EP_GL(glVertexAttribPointer(attributeLocation + i,
												vboAttributeGLTypeNumOfComponents[array][attribute],
												glBaseType,
												GL_FALSE,
												(GLsizei)vboVertexStrides[array],
												(void*)(i * sizeofShaderDataType(shaderAttributeTypes[activeShaderName][activeShaderOptions][attribute])
														+ vboAttributeOffsets[array][attribute])));
				}
				else if (glBaseType == GL_DOUBLE)
				{
					EP_GL(glVertexAttribLPointer(attributeLocation + i,
												 vboAttributeGLTypeNumOfComponents[array][attribute],
												 GL_DOUBLE,
												 (GLsizei)vboVertexStrides[array],
												 (void*)(i * sizeofShaderDataType(shaderAttributeTypes[activeShaderName][activeShaderOptions][attribute])
														 + vboAttributeOffsets[array][attribute])));
				}
				else
				{
					EP_GL(glVertexAttribIPointer(attributeLocation + i,
												 vboAttributeGLTypeNumOfComponents[array][attribute],
												 glBaseType,
												 (GLsizei)vboVertexStrides[array],
												 (void*)(i * sizeofShaderDataType(shaderAttributeTypes[activeShaderName][activeShaderOptions][attribute])
														 + vboAttributeOffsets[array][attribute])));
				}
			}
		}
	}

	// Toggle off unused attributes
	uint32_t attributesToDisableBF = (enabledAttributesBF ^ prevEnabledAttributesBF) & prevEnabledAttributesBF;
	unsigned int leastSignificantSetPosition;
	while (attributesToDisableBF != 0)
	{
		leastSignificantSetPosition = log2(attributesToDisableBF & -attributesToDisableBF);
		EP_GL(glDisableVertexAttribArray(leastSignificantSetPosition + 1));
		attributesToDisableBF &= ~(BIT(leastSignificantSetPosition));
	}
	prevEnabledAttributesBF = enabledAttributesBF;

	// Validate program before draw
#ifdef EP_CONFIG_DEBUG
	EP_GL(glValidateProgram(oglActiveProgram));
	int result;
	EP_GL(glGetProgramiv(oglActiveProgram, GL_VALIDATE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		EP_GL(glGetProgramiv(oglActiveProgram, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		EP_GL(glGetProgramInfoLog(oglActiveProgram, length, &length, message));
		EP_ERROR("[OpenGL] Program validation failure! {}", message);
	}
#endif
}

void Graphics::DrawTriangles(Graphics::VertexArrayHandle array, size_t count, size_t first)
{
	BindVertexArrayForDraw(array, count, first);

	if (iboHandles.count(array) != 0)
	{
		EP_GL(glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, (void*)(uintptr_t)first));
	}
	else
	{
		EP_GL(glDrawArrays(GL_TRIANGLES, (GLint)first, (GLsizei)count));
	}
}

void Graphics::DrawTriangleStrip(Graphics::VertexArrayHandle array, size_t count, size_t first)
{
	BindVertexArrayForDraw(array, count, first);

	if (iboHandles.count(array) != 0)
	{
		EP_GL(glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)count, GL_UNSIGNED_INT, (void*)(uintptr_t)first));
	}
	else
	{
		EP_GL(glDrawArrays(GL_TRIANGLE_STRIP, (GLint)first, (GLsizei)count));
	}
}
void Graphics::DrawLines(Graphics::VertexArrayHandle array, size_t count, size_t first)
{
	BindVertexArrayForDraw(array, count, first);

	if (iboHandles.count(array) != 0)
	{
		EP_GL(glDrawElements(GL_LINES, (GLsizei)count, GL_UNSIGNED_INT, (void*)(uintptr_t)first));
	}
	else
	{
		EP_GL(glDrawArrays(GL_LINES, (GLint)first, (GLsizei)count));
	}
}
void Graphics::DrawLineStrip(Graphics::VertexArrayHandle array, size_t count, size_t first)
{
	BindVertexArrayForDraw(array, count, first);

	if (iboHandles.count(array) != 0)
	{
		EP_GL(glDrawElements(GL_LINE_STRIP, (GLsizei)count, GL_UNSIGNED_INT, (void*)(uintptr_t)first));
	}
	else
	{
		EP_GL(glDrawArrays(GL_LINE_STRIP, (GLint)first, (GLsizei)count));
	}
}
void Graphics::DrawPoints(Graphics::VertexArrayHandle array, size_t count, size_t first)
{
	BindVertexArrayForDraw(array, count, first);

	if (iboHandles.count(array) != 0)
	{
		EP_GL(glDrawElements(GL_POINTS, (GLsizei)count, GL_UNSIGNED_INT, (void*)(uintptr_t)first));
	}
	else
	{
		EP_GL(glDrawArrays(GL_POINTS, (GLint)first, (GLsizei)count));
	}
}

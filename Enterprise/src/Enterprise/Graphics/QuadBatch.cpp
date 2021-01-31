#include "EP_PCH.h"
#include "Graphics.h"

using Enterprise::Graphics;
using namespace Enterprise::Math;

std::unordered_map<Graphics::ProgramRef, std::unordered_map<HashName, std::tuple<unsigned int, Graphics::ShaderDataType, uint64_t>>> Graphics::_quadBatchVertexInfo; // index, type, offset
std::unordered_map<Graphics::ProgramRef, unsigned int> Graphics::_quadBatchVertexStrides;

static unsigned int currentQuadCount = 0;
static unsigned int nextTextureSlot = 0;

// TODO: Protect against calls to allocate or draw basic vertex arrays in a batch.
// TODO: Protect against program changes during a batch.
// TODO: Move buffering to CPU memory, then copy to GPU all at once prior to draw call.

void oglVertexInfoFromShaderType(Graphics::ShaderDataType type, GLenum* gltype, int* paramcount)
{
	using dataType = Graphics::ShaderDataType;
	switch (type)
	{
	case dataType::Float:
		*gltype = GL_FLOAT;
		*paramcount = 1;
		break;
	case dataType::Float2:
		*gltype = GL_FLOAT;
		*paramcount = 2;
		break;
	case dataType::Float3:
		*gltype = GL_FLOAT;
		*paramcount = 3;
		break;
	case dataType::Float4:
		*gltype = GL_FLOAT;
		*paramcount = 4;
		break;
	case dataType::Int:
		*gltype = GL_INT;
		*paramcount = 1;
		break;
	case dataType::Int2:
		*gltype = GL_INT;
		*paramcount = 2;
		break;
	case dataType::Int3:
		*gltype = GL_INT;
		*paramcount = 3;
		break;
	case dataType::Int4:
		*gltype = GL_INT;
		*paramcount = 4;
		break;
	case dataType::UInt:
		*gltype = GL_UNSIGNED_INT;
		*paramcount = 1;
		break;
	case dataType::UInt2:
		*gltype = GL_UNSIGNED_INT;
		*paramcount = 2;
		break;
	case dataType::UInt3:
		*gltype = GL_UNSIGNED_INT;
		*paramcount = 3;
		break;
	case dataType::UInt4:
		*gltype = GL_UNSIGNED_INT;
		*paramcount = 4;
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}

unsigned int Graphics::_quadbatch_vbo = 0, Graphics::_quadbatch_ibo = 0;

static Vec3 basicquad_pos[4] =
{
	{ -0.5f, -0.5f, 0.0f },
	{  0.5f, -0.5f, 0.0f },
	{  0.5f,  0.5f, 0.0f },
	{ -0.5f,  0.5f, 0.0f }
};

static HashName slotAttributeNames[] =
{
	HN("ep_tex"),
	HN("ep_tex2"),
	HN("ep_tex3"),
	HN("ep_tex4"),
	HN("ep_tex5"),
	HN("ep_tex6"),
	HN("ep_tex7"),
	HN("ep_tex8"),
	HN("ep_tex9"),
	HN("ep_tex10"),
	HN("ep_tex11"),
	HN("ep_tex12"),
	HN("ep_tex13"),
	HN("ep_tex14"),
	HN("ep_tex15"),
	HN("ep_tex16"),
	HN("ep_tex17"),
	HN("ep_tex18"),
	HN("ep_tex19"),
	HN("ep_tex20"),
	HN("ep_tex21"),
	HN("ep_tex22"),
	HN("ep_tex23"),
	HN("ep_tex24"),
	HN("ep_tex25"),
	HN("ep_tex26"),
	HN("ep_tex27"),
	HN("ep_tex28"),
	HN("ep_tex29"),
	HN("ep_tex30"),
	HN("ep_tex31"),
	HN("ep_tex32")
};

// Batch quad drawing

void Graphics::QuadBatch::Begin()
{
	int textureSlotNumbers[] =
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	};
	SetUniformArray(HN("ep_texslots"), 32, ShaderDataType::Int, textureSlotNumbers);

	uint64_t newAttributeEnableStatus = 0;
	int paramcount = 0;
	GLenum gltype = 0;

	// Set the vertex layout for the current shader program
	for (auto[name, info] : _quadBatchVertexInfo[_activeProgram])
	{
		unsigned int index = std::get<0>(info);
		ShaderDataType type = std::get<1>(info);
		uint64_t offset = std::get<2>(info);

		oglVertexInfoFromShaderType(type, &gltype, &paramcount);

		// Enable the vertex attribute index, if it's not already enabled
		if ((_enabledAttributes & BIT(index)) == 0)
		{
			glEnableVertexAttribArray(index);
		}
		newAttributeEnableStatus |= BIT(index);

		// Set attribute pointers
		if (gltype == GL_FLOAT)
		{
			glVertexAttribPointer(index,
								  paramcount,
								  GL_FLOAT,
								  GL_FALSE,
								  _quadBatchVertexStrides[_activeProgram],
								  (void*)offset);
		}
		else
		{
			glVertexAttribIPointer(index,
								   paramcount,
								   gltype,
								   _quadBatchVertexStrides[_activeProgram],
								   (void*)offset);
		}
	}

	// Toggle off unused attributes
	uint64_t toTurnOff = (newAttributeEnableStatus ^ _enabledAttributes) & _enabledAttributes;
	unsigned int leastSignificantSetPosition;
	while (toTurnOff)
	{
		leastSignificantSetPosition = log2(toTurnOff & -toTurnOff);
		glDisableVertexAttribArray(leastSignificantSetPosition + 1);
		toTurnOff &= ~(BIT(leastSignificantSetPosition));
	}
	_enabledAttributes = newAttributeEnableStatus;

	// Bind the quad batch vbo and ibo.
	glBindBuffer(GL_ARRAY_BUFFER, _quadbatch_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadbatch_ibo);
	_activeArray = 0;
}

void Graphics::QuadBatch::AddQuad(Vec2 scale, Vec3 translation,
								  Vec2 uv_lowerleft, Vec2 uv_topright,
								  std::initializer_list<Graphics::TextureRef> textures)
{
	EP_ASSERT_SLOW(textures.size() > 0);
	EP_ASSERT_SLOW(textures.size() <= _maxTextureSlots);
	EP_ASSERT_SLOW(textures.size() <= 16);
	int outSlotAssignments[16] = { 0 };

	int suppliedTextureIndex = 0;
	for (TextureRef tex : textures)
	{
		int slotIndex = 0;
		for (; slotIndex < nextTextureSlot; slotIndex++)
		{
			if (tex == _textureSlots[slotIndex])
			{
				outSlotAssignments[suppliedTextureIndex] = slotIndex;
				break;
			}
		}

		if (slotIndex == nextTextureSlot)
		{
			if (nextTextureSlot == _maxTextureSlots)
			{
				// TODO: Flush batch and start new one.
				EP_ASSERT_NOENTRY();
				break;
			}
			else
			{
				// Slot the texture.
				BindTexture(tex, nextTextureSlot);
				_textureSlots[nextTextureSlot] = tex;
				outSlotAssignments[suppliedTextureIndex] = nextTextureSlot;
				nextTextureSlot++;
			}
		}
		suppliedTextureIndex++;
	}

	QuadBatchDefaultVertex outVerts[4] =
	{
		{
			basicquad_pos[0] * Mat4::Scale(scale.x, scale.y, 1.0f) * Mat4::Translation(translation),
			uv_lowerleft,
			outSlotAssignments[0]
		},
		{
			basicquad_pos[1] * Mat4::Scale(scale.x, scale.y, 1.0f) * Mat4::Translation(translation),
			{uv_topright.x, uv_lowerleft.y},
			outSlotAssignments[0]
		},
		{
			basicquad_pos[2] * Mat4::Scale(scale.x, scale.y, 1.0f) * Mat4::Translation(translation),
			uv_topright,
			outSlotAssignments[0]
		},
		{
			basicquad_pos[3] * Mat4::Scale(scale.x, scale.y, 1.0f) * Mat4::Translation(translation),
			{uv_lowerleft.x, uv_topright.y},
			outSlotAssignments[0]
		}
	};

	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 0) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[0]);
	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 1) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[1]);
	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 2) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[2]);
	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 3) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[3]);

	currentQuadCount++;

	for (int i = 1; i < textures.size(); i++)
	{
		// pass the other outSlotAssignments[] to QuadAttribute().
		QuadAttribute(slotAttributeNames[i], outSlotAssignments[i]);
	}
}

void Graphics::QuadBatch::AddRotatedQuad(Vec2 origin,
										 Vec2 scale, float roll, float pitch, float yaw, Vec3 translation,
										 Vec2 uv_lowerleft, Vec2 uv_topright,
										 std::initializer_list<Graphics::TextureRef> textures)
{
	EP_ASSERT_SLOW(textures.size() > 0);
	EP_ASSERT_SLOW(textures.size() <= _maxTextureSlots);
	EP_ASSERT_SLOW(textures.size() <= 16);
	int outSlotAssignments[16] = { 0 };

	int suppliedTextureIndex = 0;
	for (TextureRef tex : textures)
	{
		int slotIndex = 0;
		for (; slotIndex < nextTextureSlot; slotIndex++)
		{
			if (tex == _textureSlots[slotIndex])
			{
				outSlotAssignments[suppliedTextureIndex] = slotIndex;
				break;
			}
		}

		if (slotIndex == nextTextureSlot)
		{
			if (nextTextureSlot == _maxTextureSlots)
			{
				// TODO: Flush batch and start new one.
				EP_ASSERT_NOENTRY();
				break;
			}
			else
			{
				// Slot the texture.
				BindTexture(tex, nextTextureSlot);
				_textureSlots[nextTextureSlot] = tex;
				outSlotAssignments[suppliedTextureIndex] = nextTextureSlot;
				nextTextureSlot++;
			}
		}
		suppliedTextureIndex++;
	}

	QuadBatchDefaultVertex outVerts[4] =
	{
		{
			basicquad_pos[0]
			* Mat4::Translation(Vec3(-origin.x, -origin.y, 0.0f))
			* Mat4::Scale(scale.x, scale.y, 1.0f)
			* Mat4::Rotation(roll, pitch, yaw)
			* Mat4::Translation(translation),
			uv_lowerleft,
			outSlotAssignments[0]
		},
		{
			basicquad_pos[1]
			* Mat4::Translation(Vec3(-origin.x, -origin.y, 0.0f))
			* Mat4::Scale(scale.x, scale.y, 1.0f)
			* Mat4::Rotation(roll, pitch, yaw)
			* Mat4::Translation(translation),
			{uv_topright.x, uv_lowerleft.y},
			outSlotAssignments[0]
		},
		{
			basicquad_pos[2]
			* Mat4::Translation(Vec3(-origin.x, -origin.y, 0.0f))
			* Mat4::Scale(scale.x, scale.y, 1.0f)
			* Mat4::Rotation(roll, pitch, yaw)
			* Mat4::Translation(translation),
			uv_topright,
			outSlotAssignments[0]
		},
		{
			basicquad_pos[3]
			* Mat4::Translation(Vec3(-origin.x, -origin.y, 0.0f))
			* Mat4::Scale(scale.x, scale.y, 1.0f)
			* Mat4::Rotation(roll, pitch, yaw)
			* Mat4::Translation(translation),
			{uv_lowerleft.x, uv_topright.y},
			outSlotAssignments[0]
		}
	};

	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 0) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[0]);
	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 1) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[1]);
	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 2) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[2]);
	glBufferSubData(GL_ARRAY_BUFFER,
					((uint64_t)currentQuadCount * 4 + 3) * _quadBatchVertexStrides[_activeProgram],
					sizeof(QuadBatchDefaultVertex),
					&outVerts[3]);

	currentQuadCount++;

	for (int i = 1; i < textures.size(); i++)
	{
		// pass the other outSlotAssignments[] to QuadAttribute().
		QuadAttribute(slotAttributeNames[i], outSlotAssignments[i]);
	}
}

void Graphics::QuadBatch::QuadAttribute(HashName attribute, float value)
{
}
void Graphics::QuadBatch::VertexAttribute(uint_fast8_t vertex, HashName attribute, float value)
{
}

void Graphics::QuadBatch::End()
{
	// Pipeline check
	#ifdef EP_CONFIG_DEBUG
	glValidateProgram(_activeProgram);
	int result;
	glGetProgramiv(_activeProgram, GL_VALIDATE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetProgramiv(_activeProgram, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(_activeProgram, length, &length, message);
		EP_ERROR(" [OpenGL] Program validation failure! {}", message);
	}
	#endif

	glDrawElements(GL_TRIANGLES, currentQuadCount * 6, GL_UNSIGNED_INT, nullptr);

	currentQuadCount = 0;
	nextTextureSlot = 0;
}

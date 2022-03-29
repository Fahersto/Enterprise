#include "Enterprise/Graphics/OpenGLHelpers.h"

using Enterprise::Graphics;

ShaderDataType oglTypeToShaderDataType(GLenum glType)
{
	switch (glType)
	{
		case GL_FLOAT:
			return ShaderDataType::Float;
			break;
		case GL_FLOAT_VEC2:
			return ShaderDataType::Vec2;
			break;
		case GL_FLOAT_VEC3:
			return ShaderDataType::Vec3;
			break;
		case GL_FLOAT_VEC4:
			return ShaderDataType::Vec4;
			break;
		case GL_INT:
			return ShaderDataType::Int;
			break;
		case GL_SAMPLER_1D:
			return ShaderDataType::Int;
			break;
		case GL_SAMPLER_2D:
			return ShaderDataType::Int;
			break;
		case GL_SAMPLER_3D:
			return ShaderDataType::Int;
			break;
		case GL_UNSIGNED_INT:
			return ShaderDataType::UInt;
			break;
		case GL_FLOAT_MAT3:
			return ShaderDataType::Mat3;
			break;
		case GL_FLOAT_MAT4:
			return ShaderDataType::Mat4;
			break;
		default:
			EP_ASSERT_NOENTRY();
			return ShaderDataType::none;
			break;
	}
}

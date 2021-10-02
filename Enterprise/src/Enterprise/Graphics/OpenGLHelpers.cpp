#include "EP_PCH.h"
#include "OpenGLHelpers.h"

using Enterprise::Graphics;

Graphics::ShaderDataType oglTypeToShaderDataType(GLenum glType)
{
	switch (glType)
	{
		case GL_FLOAT:
			return Graphics::ShaderDataType::Float;
			break;
		case GL_FLOAT_VEC2:
			return Graphics::ShaderDataType::Vec2;
			break;
		case GL_FLOAT_VEC3:
			return Graphics::ShaderDataType::Vec3;
			break;
		case GL_FLOAT_VEC4:
			return Graphics::ShaderDataType::Vec4;
			break;
		case GL_INT:
			return Graphics::ShaderDataType::Int;
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

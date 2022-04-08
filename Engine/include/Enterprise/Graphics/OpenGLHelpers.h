#pragma once
#include <glad/glad.h>
#include "Enterprise/Core.h"
#include "Enterprise/Graphics.h"

#ifdef EP_CONFIG_DEBUG

#define EP_GL(code) code;\
{ \
	GLenum err; \
	while((err = glGetError()) != GL_NO_ERROR) \
	{ \
		switch(err) \
		{ \
			case GL_INVALID_ENUM: \
				EP_ERROR("[OpenGL] GL_INVALID_ENUM File: {} Line: {}", __FILE__, __LINE__); \
				break; \
			case GL_INVALID_VALUE: \
				EP_ERROR("[OpenGL] GL_INVALID_VALUE File: {} Line: {}", __FILE__, __LINE__); \
				break; \
			case GL_INVALID_OPERATION: \
				EP_ERROR("[OpenGL] GL_INVALID_OPERATION File: {} Line: {}", __FILE__, __LINE__); \
				break; \
			case GL_OUT_OF_MEMORY: \
				EP_ERROR("[OpenGL] GL_OUT_OF_MEMORY File: {} Line: {}", __FILE__, __LINE__); \
				break; \
			case GL_INVALID_FRAMEBUFFER_OPERATION: \
				EP_ERROR("[OpenGL] GL_INVALID_FRAMEBUFFER_OPERATION File: {} Line: {}", __FILE__, __LINE__); \
				break; \
			default: \
				EP_ERROR("[OpenGL] Unhandled error! Type: {} File: {} Line: {}", err, __FILE__, __LINE__); \
				break; \
		} \
		EP_DEBUGBREAK(); \
	} \
}

#else

#define EP_GL(code) code;

#endif // Debug / non-debug

ShaderDataType oglTypeToShaderDataType(GLenum glType);

inline GLint imageFmtToOGLInternalFmt(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::RGB8:
		return GL_RGB8;
		break;
	case ImageFormat::RGB16F:
		return GL_RGB16F;
		break;
	case ImageFormat::RGB32F:
		return GL_RGB32F;
		break;
	case ImageFormat::Depth16:
		return GL_DEPTH_COMPONENT16;
		break;
	case ImageFormat::Depth24:
		return GL_DEPTH_COMPONENT24;
		break;
	case ImageFormat::Depth32:
		return GL_DEPTH_COMPONENT32;
		break;
	case ImageFormat::Depth32F:
		return GL_DEPTH_COMPONENT32F;
		break;
	case ImageFormat::Depth24Stencil8:
		return GL_DEPTH_STENCIL;
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}
inline GLenum imageFmtToOGLImageFmt(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::RGB8:
		return GL_RGB;
		break;
	case ImageFormat::RGB16F:
		return GL_RGB;
		break;
	case ImageFormat::RGB32F:
		return GL_RGB;
		break;
	case ImageFormat::Depth16:
		return GL_DEPTH_COMPONENT;
		break;
	case ImageFormat::Depth24:
		return GL_DEPTH_COMPONENT;
		break;
	case ImageFormat::Depth32:
		return GL_DEPTH_COMPONENT;
		break;
	case ImageFormat::Depth32F:
		return GL_DEPTH_COMPONENT;
		break;
	case ImageFormat::Depth24Stencil8:
		return GL_DEPTH_STENCIL;
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}
inline GLenum imageFmtToOGLPixelDataType(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::RGB8:
		return GL_UNSIGNED_BYTE;
		break;
	case ImageFormat::RGB16F:
		return GL_FLOAT;
		break;
	case ImageFormat::RGB32F:
		return GL_FLOAT;
		break;
	case ImageFormat::Depth16:
		return GL_UNSIGNED_SHORT;
		break;
	case ImageFormat::Depth24:
		return GL_UNSIGNED_INT;
		break;
	case ImageFormat::Depth32:
		return GL_UNSIGNED_INT;
		break;
	case ImageFormat::Depth32F:
		return GL_FLOAT;
		break;
	case ImageFormat::Depth24Stencil8:
		return GL_UNSIGNED_INT_24_8;
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}

inline GLint texFilterToOGLFilter(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::Nearest:
		return GL_NEAREST;
		break;
	//case TextureFilter::Anisotropic:
	//	break;
	case TextureFilter::Linear:
		return GL_LINEAR;
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}

#pragma once
#include "EP_PCH.h"
#include "Core.h"
#include "Graphics.h"

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

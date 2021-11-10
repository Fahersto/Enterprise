#include "EP_PCH.h"
#include "Graphics.h"
#include "Enterprise/File/File.h"

#include "OpenGLHelpers.h"

using Enterprise::Graphics;

static std::map<HashName, int> textureReferenceCount;
static std::map<HashName, Graphics::TextureHandle> textureHandlesByPath;
static std::map<Graphics::TextureHandle, HashName> pathOfTextureHandle;

// externs defined in Shaders.cpp
extern std::map<HashName, std::map<std::set<HashName>, std::map<HashName, GLint>>> samplerUniformTypes; // Key 1: Program name Key 2: Options Key 3: Uniform name
extern std::map<HashName, std::map<std::set<HashName>, std::map<HashName, GLint>>> samplerUniformLocations;
static std::map<HashName, std::map<std::set<HashName>, std::map<HashName, GLint>>> currentSamplerAssignment;
static std::map<Graphics::TextureHandle, GLint> samplerTypeNeededForTexture;

std::map<Graphics::TextureHandle, int> Graphics::currentSlotOfTexture;

Graphics::TextureHandle Graphics::LoadTexture(std::string path, TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode)
{
	GLuint texture = 0;
	if (textureReferenceCount[HN(path)] == 0)
	{
		if (File::Exists(path))
		{
			// Load image file into local memory
			stbi_set_flip_vertically_on_load(1);
			int width, height, numOfColorComponents;
			unsigned char* buffer;

			bool is16Bit = stbi_is_16_bit(File::VirtualPathToNative(path).c_str());
			if (!is16Bit)
				buffer = stbi_load(File::VirtualPathToNative(path).c_str(), &width, &height, &numOfColorComponents, 0);
			else
				buffer = (unsigned char*)stbi_load_16(File::VirtualPathToNative(path).c_str(), &width, &height, &numOfColorComponents, 0);

			if (buffer)
			{
				GLenum textureTarget;
				(width == 1 || height == 1) ? textureTarget = GL_TEXTURE_1D : textureTarget = GL_TEXTURE_2D;

				EP_GL(glGenTextures(1, &texture));
				EP_GL(glBindTexture(textureTarget, texture));

				switch (minFilter)
				{
					case TextureFilter::Nearest:
						switch(mipmapMode)
						{
							case MipmapMode::None:
								EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
								break;
							case MipmapMode::Nearest:
								EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
								break;
							case MipmapMode::Linear:
								EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
								break;
							default:
								EP_ASSERT_NOENTRY();
								break;
						}
						break;
					case TextureFilter::Linear:
						switch(mipmapMode)
						{
							case MipmapMode::None:
								EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
								break;
							case MipmapMode::Nearest:
								EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
								break;
							case MipmapMode::Linear:
								EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
								break;
							default:
								EP_ASSERT_NOENTRY();
								break;
						}
						break;
						//		case TextureFilter::Anisotropic:
						//			break;
					default:
						EP_ASSERT_NOENTRY();
						break;
				}
				switch (magFilter)
				{
					case TextureFilter::Nearest:
						EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
						break;
					case TextureFilter::Linear:
						EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
						break;
						//		case TextureFilter::Anisotropic:
						//			break;
					default:
						EP_ASSERT_NOENTRY();
						break;
				}

				// TODO: Support other wrap modes
				EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				EP_GL(glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

				// Send image to GPU
				if (textureTarget == GL_TEXTURE_1D)
				{
					switch(numOfColorComponents)
					{
						case 1:
							EP_GL(glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, height == 1 ? width : height, 0, GL_RED, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						case 2:
							EP_GL(glTexImage1D(GL_TEXTURE_1D, 0, GL_RG, height == 1 ? width : height, 0, GL_RG, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						case 3:
							EP_GL(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, height == 1 ? width : height, 0, GL_RGB, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						case 4:
							EP_GL(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, height == 1 ? width : height, 0, GL_RGBA, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						default:
							EP_ASSERT_NOENTRY();
							break;
					}
					samplerTypeNeededForTexture[texture] = GL_SAMPLER_1D;
				}
				else
				{
					switch(numOfColorComponents)
					{
						case 1:
							EP_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						case 2:
							EP_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						case 3:
							EP_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						case 4:
							EP_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, is16Bit ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, buffer));
							break;
						default:
							EP_ASSERT_NOENTRY();
							break;
					}
					samplerTypeNeededForTexture[texture] = GL_SAMPLER_2D;
				}

				// Generate mipmaps
				if (mipmapMode != MipmapMode::None)
				{
#ifndef EP_CONFIG_DIST
					if (((width & (width - 1)) != 0) || ((height & (height - 1)) != 0))
					{
						EP_WARN("Graphics::LoadTexture(): Mipmaps generated for non-power-of-two texture \"{}\".", path);
					}
#endif
					EP_GL(glGenerateMipmap(textureTarget));
				}

				// Unbind texture and release local buffer
				EP_GL(glBindTexture(textureTarget, 0));
				stbi_image_free(buffer);

				currentSlotOfTexture[texture] = -1;
				textureHandlesByPath[HN(path)] = texture;
				pathOfTextureHandle[texture] = HN(path);
			}
			else
			{
				EP_WARN("Graphics::LoadTexture(): stbi_load() failure!  File: \"{}\"", File::VirtualPathToNative(path));
				textureReferenceCount.erase(HN("path"));
			}
		}
		else
		{
			textureReferenceCount.erase(HN("path"));
		}
	}
	else
	{
		texture = textureHandlesByPath[HN(path)];
	}
	textureReferenceCount[HN(path)]++;
	return texture;
}
Graphics::TextureHandle Graphics::LoadCubemapTexture(std::string path, TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode)
{
	// TODO: Implement
	return 0;
}
Graphics::TextureHandle Graphics::LoadTextureArray(std::initializer_list<std::string> paths, TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode)
{
	// TODO: Implement
	return 0;
}
Graphics::TextureHandle Graphics::LoadTextureArray(std::string path, TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode, size_t columns, size_t rows, size_t count)
{
	// TODO: Implement
	return 0;
}
Graphics::TextureHandle Graphics::LoadCubemapTextureArray(std::initializer_list<std::string> paths, TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode)
{
	// TODO: Implement
	return 0;
}
Graphics::TextureHandle Graphics::Load3DTexture(std::string path, TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode, size_t columns, size_t rows, size_t depth)
{
	// TODO: Implement
	return 0;
}


void Graphics::DeleteTexture(Graphics::TextureHandle texture)
{
	EP_ASSERT(texture);
	EP_ASSERT(pathOfTextureHandle.count(texture));
	EP_ASSERT(textureReferenceCount.count(pathOfTextureHandle[texture]));

	textureReferenceCount[pathOfTextureHandle[texture]]--;
	EP_ASSERT(textureReferenceCount[pathOfTextureHandle[texture]] >= 0);

	if (textureReferenceCount[pathOfTextureHandle[texture]] == 0)
	{
		if (currentSlotOfTexture[texture] != -1)
		{
			isTextureSlotUsedThisDraw[currentSlotOfTexture[texture]] = false;
			textureInSlot[currentSlotOfTexture[texture]] = 0;
			currentSlotOfTexture[texture] = -1;
		}
		pathOfTextureHandle.erase(texture);
		EP_GL(glDeleteTextures(1, &texture));
	}
}

void Graphics::BindTexture(Graphics::TextureHandle texture, HashName uniform)
{
	EP_ASSERT_SLOW(textureReferenceCount[pathOfTextureHandle[texture]] > 0);

	// Bind texture to slot if needed
	EP_ASSERT_SLOW(currentSlotOfTexture.count(texture) != 0);
	if (currentSlotOfTexture[texture] == -1) // Not currently bound to a slot
	{
		int i = lastBoundTextureSlot;
		for ( ; i < maxTextureSlots + lastBoundTextureSlot; i++)
		{
			if (!isTextureSlotUsedThisDraw[i % maxTextureSlots])
			{
				EP_GL(glActiveTexture(GL_TEXTURE0 + (i % maxTextureSlots)));

				switch (samplerTypeNeededForTexture[texture])
				{
					case GL_SAMPLER_1D:
						EP_GL(glBindTexture(GL_TEXTURE_1D, texture));
						break;
					case GL_INT_SAMPLER_1D:
						EP_GL(glBindTexture(GL_TEXTURE_1D, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_1D:
						EP_GL(glBindTexture(GL_TEXTURE_1D, texture));
						break;
					case GL_SAMPLER_1D_SHADOW:
						EP_GL(glBindTexture(GL_TEXTURE_1D, texture));
						break;

					case GL_SAMPLER_2D:
						EP_GL(glBindTexture(GL_TEXTURE_2D, texture));
						break;
					case GL_INT_SAMPLER_2D:
						EP_GL(glBindTexture(GL_TEXTURE_2D, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_2D:
						EP_GL(glBindTexture(GL_TEXTURE_2D, texture));
						break;
					case GL_SAMPLER_2D_SHADOW:
						EP_GL(glBindTexture(GL_TEXTURE_2D, texture));
						break;

					case GL_SAMPLER_3D:
						EP_GL(glBindTexture(GL_TEXTURE_3D, texture));
						break;
					case GL_INT_SAMPLER_3D:
						EP_GL(glBindTexture(GL_TEXTURE_3D, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_3D:
						EP_GL(glBindTexture(GL_TEXTURE_3D, texture));
						break;

					case GL_SAMPLER_CUBE:
						EP_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
						break;
					case GL_INT_SAMPLER_CUBE:
						EP_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_CUBE:
						EP_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
						break;
					case GL_SAMPLER_CUBE_SHADOW:
						EP_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
						break;

					case GL_SAMPLER_1D_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_1D_ARRAY, texture));
						break;
					case GL_INT_SAMPLER_1D_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_1D_ARRAY, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_1D_ARRAY, texture));
						break;
					case GL_SAMPLER_1D_ARRAY_SHADOW:
						EP_GL(glBindTexture(GL_TEXTURE_1D_ARRAY, texture));
						break;

					case GL_SAMPLER_2D_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_2D_ARRAY, texture));
						break;
					case GL_INT_SAMPLER_2D_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_2D_ARRAY, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_2D_ARRAY, texture));
						break;
					case GL_SAMPLER_2D_ARRAY_SHADOW:
						EP_GL(glBindTexture(GL_TEXTURE_2D_ARRAY, texture));
						break;

					case GL_SAMPLER_2D_MULTISAMPLE:
						EP_GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture));
						break;
					case GL_INT_SAMPLER_2D_MULTISAMPLE:
						EP_GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
						EP_GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture));
						break;

					case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, texture));
						break;
					case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
						EP_GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, texture));
						break;

					case GL_SAMPLER_BUFFER:
						EP_GL(glBindTexture(GL_TEXTURE_BUFFER, texture));
						break;
					case GL_INT_SAMPLER_BUFFER:
						EP_GL(glBindTexture(GL_TEXTURE_BUFFER, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_BUFFER:
						EP_GL(glBindTexture(GL_TEXTURE_BUFFER, texture));
						break;

					case GL_SAMPLER_2D_RECT:
						EP_GL(glBindTexture(GL_TEXTURE_RECTANGLE, texture));
						break;
					case GL_INT_SAMPLER_2D_RECT:
						EP_GL(glBindTexture(GL_TEXTURE_RECTANGLE, texture));
						break;
					case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
						EP_GL(glBindTexture(GL_TEXTURE_RECTANGLE, texture));
						break;
					case GL_SAMPLER_2D_RECT_SHADOW:
						EP_GL(glBindTexture(GL_TEXTURE_RECTANGLE, texture));
						break;
				}

				currentSlotOfTexture[textureInSlot[i % maxTextureSlots]] = -1;
				currentSlotOfTexture[texture] = i % maxTextureSlots;
				textureInSlot[i % maxTextureSlots] = texture;
				lastBoundTextureSlot = i % maxTextureSlots;
				isTextureSlotUsedThisDraw[i % maxTextureSlots] = true;
				break;
			}
		}
		if (i == maxTextureSlots + lastBoundTextureSlot)
		{
			EP_ERROR("Graphics::BindTexture(): Exhausted all available texture slots!");
			return;
		}
	}

	// Ensure that sampler uniform exists within shader
	if (samplerUniformTypes[activeShaderName][activeShaderOptions].count(uniform) == 0)
	{
		EP_ERROR("Graphics::BindTexture(): Sampler uniform does not exist!  "
				 "Program: {}, Sampler: {}", HN_ToStr(activeShaderName), HN_ToStr(uniform));
		return;
	}

	// Ensure that sampler type and texture match
	if (samplerUniformTypes[activeShaderName][activeShaderOptions][uniform] != samplerTypeNeededForTexture[texture])
	{
		EP_ERROR("Graphics::BindTexture(): Texture type does not match sampler type!  "
				 "Program: {}, Sampler: {}", HN_ToStr(activeShaderName), HN_ToStr(uniform));
		return;
	}

	// Update sampler uniform if needed
	if (currentSamplerAssignment[activeShaderName][activeShaderOptions][uniform] != currentSlotOfTexture[texture])
	{
		EP_GL(glUniform1i(samplerUniformLocations[activeShaderName][activeShaderOptions][uniform], currentSlotOfTexture[texture]));
		currentSamplerAssignment[activeShaderName][activeShaderOptions][uniform] = currentSlotOfTexture[texture];
	}
}

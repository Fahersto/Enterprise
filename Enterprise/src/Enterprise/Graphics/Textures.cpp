#include "EP_PCH.h"
#include "Graphics.h"
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "Enterprise/File/File.h"
#include "OpenGLHelpers.h"

using Enterprise::Graphics;

static std::map<HashName, int> textureReferenceCount;
static std::map<HashName, Graphics::TextureHandle> textureHandlesByPath;
static std::map<Graphics::TextureHandle, HashName> pathOfTextureHandle;

// externs defined in Shaders.cpp
extern std::map<HashName, std::map<std::set<HashName>, std::map<HashName, GLint>>> samplerUniformTypes; // Key 1: Program name Key 2: Options Key 3: Uniform name
extern std::map<HashName, std::map<std::set<HashName>, std::map<HashName, std::vector<GLint>>>> samplerUniformLocations;
static std::map<HashName, std::map<std::set<HashName>, std::map<HashName, std::vector<GLint>>>> currentSamplerAssignment;
std::map<Graphics::TextureHandle, GLint> samplerTypeNeededForTexture; // Referenced in Framebuffers.cpp as extern

std::map<Graphics::TextureHandle, int> Graphics::currentSlotOfTexture;

//static std::map<HashName, unsigned int> msdfReferenceCount;
static std::map<HashName, Graphics::TextureHandle> msdfTextureHandles;
static std::map<Graphics::TextureHandle, std::map<std::pair<uint32_t, uint32_t>, double>> fontKerns;
static std::map<Graphics::TextureHandle, std::map<uint32_t, double>> fontAdvances;
static std::map<Graphics::TextureHandle, std::tuple<double, double, double>> fontVerticalMetrics; // line height, ascender height, descender height
static std::map<Graphics::TextureHandle, std::map<uint32_t, std::tuple<float, float, float, float>>> fontUVBounds;
static std::map<Graphics::TextureHandle, std::map<uint32_t, std::tuple<float, float, float, float>>> fontQuadBounds;

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
				textureReferenceCount.erase(HN(path));
			}
		}
		else
		{
			textureReferenceCount.erase(HN(path));
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

#define DEFAULT_PIXEL_RANGE 2.0
#define DEFAULT_MITER_LIMIT 1.0
#define DEFAULT_ANGLE_THRESHOLD 3.0

Graphics::TextureHandle Graphics::LoadFontFile(const std::string& path, float emSizeMin)
{
	if (textureReferenceCount[HN(path)] > 0)
	{
		textureReferenceCount[HN(path)]++;
		return msdfTextureHandles[HN(path)];
	}
	else if (File::Exists(path))
	{
		std::vector<msdf_atlas::GlyphGeometry> glyphs;
		msdf_atlas::FontGeometry fontGeometry(&glyphs);

		// Load font
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		if (ft)
		{
			msdfgen::FontHandle* font = msdfgen::loadFont(ft, File::VirtualPathToNative(path).c_str());
			if (font)
			{
				int glyphsLoaded = fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);
				if (glyphsLoaded < 0)
				{
					EP_ERROR("Graphics::LoadFontFile(): Could not load glyphs from font \"{}\"!",
						File::VirtualPathToNative(path));
					return Graphics::TextureHandle();
				}

				if (glyphsLoaded < msdf_atlas::Charset::ASCII.size())
				{
					EP_WARN("Graphics::LoadFontFile(): Font \"{}\" is missing {} codepoints!  Codepoints:",
						File::VirtualPathToNative(path),
						msdf_atlas::Charset::ASCII.size() - glyphsLoaded);
					bool first = true;
					for (msdfgen::unicode_t cp : msdf_atlas::Charset::ASCII)
					{
						if (!fontGeometry.getGlyph(cp))
							printf("%c 0x%02X", first ? ((first = false), ':') : ',', cp);
					}
					printf("\n");
				}

				msdfgen::destroyFont(font);
			}
			msdfgen::deinitializeFreetype(ft);
		}

		// Assign glyph edge colors
		if (glyphs.empty())
		{
			EP_ERROR("Graphics::LoadFontFile(): No glyphs loaded from font \"{}\"!",
				File::VirtualPathToNative(path));
			return Graphics::TextureHandle();
		}
		else
		{
			for (msdf_atlas::GlyphGeometry& glyph : glyphs)
			{
				glyph.edgeColoring(msdfgen::edgeColoringByDistance, DEFAULT_ANGLE_THRESHOLD, 1);
			}
		}

		// Pack glyphs
		msdf_atlas::TightAtlasPacker atlasPacker;
		atlasPacker.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_RECTANGLE);
		atlasPacker.setMinimumScale(emSizeMin);
		atlasPacker.setPixelRange(DEFAULT_PIXEL_RANGE);
		atlasPacker.setMiterLimit(DEFAULT_MITER_LIMIT);

		int glyphPackFailureCount = atlasPacker.pack(glyphs.data(), glyphs.size());
		if (glyphPackFailureCount > 0)
		{
			EP_WARN("Graphics::LoadFontFile(): Failure to pack glyphs from font \"{}\" into atlas!  "
				"Unpacked glyphs : {}", File::VirtualPathToNative(path), glyphPackFailureCount);
		}

		int width = -1, height = -1;
		atlasPacker.getDimensions(width, height);

		// Generate the MSDF
		msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator,
			msdf_atlas::BitmapAtlasStorage<unsigned char, 3>> generator(width, height);
		//msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator,
		//	msdf_atlas::BitmapAtlasStorage<unsigned char, 4>> generator(width, height);
		generator.setThreadCount(std::max((int)std::thread::hardware_concurrency(), 1));
		generator.generate(glyphs.data(), glyphs.size());
		auto bitmap = (msdfgen::BitmapConstRef<unsigned char, 3>)generator.atlasStorage();

		// Send to the GPU
		GLuint texHandleOut = 0;
		if (bitmap.width * bitmap.height > 0)
		{
			EP_GL(glGenTextures(1, &texHandleOut));
			EP_GL(glBindTexture(GL_TEXTURE_2D, texHandleOut));

			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			// TODO: Add support for anisotropic filtering
			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			EP_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.pixels));
			//EP_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.pixels));

			EP_GL(glBindTexture(GL_TEXTURE_2D, 0));

			samplerTypeNeededForTexture[texHandleOut] = GL_SAMPLER_2D;
			currentSlotOfTexture[texHandleOut] = -1;
			textureHandlesByPath[HN(path)] = texHandleOut;
			pathOfTextureHandle[texHandleOut] = HN(path);
		}

		// Store font metrics
		std::map<int, uint32_t> indexToCodePoint;
		for (const msdf_atlas::GlyphGeometry& glyph : glyphs)
		{
			indexToCodePoint[glyph.getIndex()] = glyph.getCodepoint();
			fontAdvances[texHandleOut][glyph.getCodepoint()] = glyph.getAdvance();

			double l, b, r, t;
			glyph.getQuadAtlasBounds(l, b, r, t);
			l /= bitmap.width;
			r /= bitmap.width;
			b /= bitmap.height;
			t /= bitmap.height;
			fontUVBounds[texHandleOut][glyph.getCodepoint()] = std::tuple(l, r, b, t);

			glyph.getQuadPlaneBounds(l, b, r, t);
			fontQuadBounds[texHandleOut][glyph.getCodepoint()] = std::tuple(l, r, b, t);
		}
		
		msdfgen::FontMetrics metrics = fontGeometry.getMetrics();
		fontVerticalMetrics[texHandleOut] = std::tuple
		(
			metrics.lineHeight,
			metrics.ascenderY,
			-metrics.descenderY
		);

		for (auto& [indicesKey, kernVal] : fontGeometry.getKerning())
		{
			std::pair<uint32_t, uint32_t> codePointsKey
			(
				indexToCodePoint[indicesKey.first],
				indexToCodePoint[indicesKey.second]
			);
			fontKerns[texHandleOut][codePointsKey] = kernVal;
		}

		textureReferenceCount[HN(path)]++;
		pathOfTextureHandle[texHandleOut] = HN(path);
		return texHandleOut;
	}
	else
	{
		EP_ERROR("Graphics::LoadFontFile(): \"{}\" does not exist!", File::VirtualPathToNative(path));
		return 0;
	}
}


void Graphics::GetFontCharUVBounds(TextureHandle atlas, uint32_t unicodeChar,
	float& out_l, float& out_r, float& out_b, float& out_t)
{
	if (fontUVBounds.count(atlas) > 0)
	{
		if (fontUVBounds[atlas].count(unicodeChar) > 0)
		{
			out_l = std::get<0>(fontUVBounds[atlas][unicodeChar]);
			out_r = std::get<1>(fontUVBounds[atlas][unicodeChar]);
			out_b = std::get<2>(fontUVBounds[atlas][unicodeChar]);
			out_t = std::get<3>(fontUVBounds[atlas][unicodeChar]);
		}
		else
		{
			EP_WARN("Graphics::GetFontCharUVBounds(): Font does not contain character {0:x}!  "
				"TextureHandle: {1:d}", unicodeChar, atlas);
		}
	}
	else
	{
		EP_WARN("Graphics::GetFontCharUVBounds(): TextureHandle 'atlas' is not associated with a loaded font!  "
			"TextureHandle: {}", atlas);
	}
}

void Graphics::GetFontCharQuadBounds(TextureHandle atlas, uint32_t unicodeChar,
	float& out_l, float& out_r, float& out_b, float& out_t, uint32_t prevChar)
{
	if (fontQuadBounds.count(atlas) > 0)
	{
		if (fontQuadBounds[atlas].count(unicodeChar) > 0)
		{
			out_l = std::get<0>(fontQuadBounds[atlas][unicodeChar]);
			out_r = std::get<1>(fontQuadBounds[atlas][unicodeChar]);
			out_b = std::get<2>(fontQuadBounds[atlas][unicodeChar]);
			out_t = std::get<3>(fontQuadBounds[atlas][unicodeChar]);

			if (fontKerns[atlas].count(std::pair(unicodeChar, prevChar)) > 0)
			{
				out_l += fontKerns[atlas][std::pair(unicodeChar, prevChar)];
			}
		}
		else
		{
			EP_WARN("Graphics::GetFontCharQuadBounds(): Font does not contain character {0:x}!  "
				"TextureHandle: {1:d}", unicodeChar, atlas);
		}
	}
	else
	{
		EP_WARN("Graphics::GetFontCharQuadBounds(): TextureHandle 'atlas' is not associated with a loaded font!  "
			"TextureHandle: {}", atlas);
	}
}

double Graphics::GetFontCharAdvance(TextureHandle atlas, uint32_t unicodeChar)
{
	if (fontAdvances.count(atlas) > 0)
	{
		if (fontAdvances[atlas].count(unicodeChar) > 0)
		{
			return fontAdvances[atlas][unicodeChar];
		}
		else
		{
			EP_WARN("Graphics::GetFontCharAdvance(): Font does not contain character {0:x}!  "
				"TextureHandle: {1:d}", unicodeChar, atlas);
			return 0.0;
		}
	}
	else
	{
		EP_WARN("Graphics::GetFontCharAdvance(): TextureHandle 'atlas' is not associated with a loaded font!  "
			"TextureHandle: {}", atlas);
		return 0.0;
	}
}

void Graphics::GetFontVerticalMetrics(TextureHandle atlas, double& out_lineHeight, double& out_ascenderHeight, double& out_descenderHeight)
{
	if (fontVerticalMetrics.count(atlas) > 0)
	{
		out_lineHeight = std::get<0>(fontVerticalMetrics[atlas]);
		out_ascenderHeight = std::get<1>(fontVerticalMetrics[atlas]);
		out_descenderHeight = std::get<2>(fontVerticalMetrics[atlas]);
	}
	else
	{
		EP_WARN("Graphics::GetFontLineHeight(): TextureHandle 'atlas' is not associated with a loaded font!  "
			"TextureHandle: {}", atlas);
	}
}

HashName Graphics::GetTextureHashedPath(TextureHandle texture)
{
	if (pathOfTextureHandle.count(texture))
	{
		return pathOfTextureHandle[texture];
	}
	else
	{
		EP_WARN("Graphics::GetTextureHashedPath(): 'texture' is not a valid texture handle!");
		return HN_NULL;
	}
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
		// Delete font data, if texture is a font atlas
		if (fontAdvances.count(texture))
		{
			msdfTextureHandles.erase(pathOfTextureHandle[texture]);
			fontKerns.erase(texture);
			fontAdvances.erase(texture);
			fontVerticalMetrics.erase(texture);
			fontUVBounds.erase(texture);
			fontQuadBounds.erase(texture);
		}

		// Unbind texture, if it's bound
		if (currentSlotOfTexture[texture] != -1)
		{
			isTextureSlotUsedThisDraw[currentSlotOfTexture[texture]] = false;
			textureInSlot[currentSlotOfTexture[texture]] = 0;
			currentSlotOfTexture[texture] = -1;
		}

		textureHandlesByPath.erase(pathOfTextureHandle[texture]);
		pathOfTextureHandle.erase(texture);
		EP_GL(glDeleteTextures(1, &texture));
	}
}

void Graphics::BindTexture(Graphics::TextureHandle texture, HashName uniform, unsigned int index)
{
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
		if (activeShaderName != HN("EPNULLSHADER"))
		{
			EP_ERROR("Graphics::BindTexture(): Sampler uniform does not exist!  "
				"Program: {}, Sampler: {}", HN_ToStr(activeShaderName), HN_ToStr(uniform));
			EP_DEBUGBREAK();
		}
		return;
	}

	// Ensure that sampler type and texture match
	if (samplerUniformTypes[activeShaderName][activeShaderOptions][uniform] != samplerTypeNeededForTexture[texture])
	{
		EP_ERROR("Graphics::BindTexture(): Texture type does not match sampler type!  "
				 "Program: {}, Sampler: {}", HN_ToStr(activeShaderName), HN_ToStr(uniform));
		return;
	}

	if (index < samplerUniformLocations[activeShaderName][activeShaderOptions][uniform].size())
	{
		// Update sampler uniform if needed
		while (index >= currentSamplerAssignment[activeShaderName][activeShaderOptions][uniform].size())
		{
			currentSamplerAssignment[activeShaderName][activeShaderOptions][uniform].push_back(-1);
		}

		if (currentSamplerAssignment[activeShaderName][activeShaderOptions][uniform][index] != currentSlotOfTexture[texture])
		{
			EP_GL(glUniform1i(samplerUniformLocations[activeShaderName][activeShaderOptions][uniform][index], currentSlotOfTexture[texture]));
			currentSamplerAssignment[activeShaderName][activeShaderOptions][uniform][index] = currentSlotOfTexture[texture];
		}
	}
	else
	{
		EP_ERROR("Graphics::BindTexture(): Sampler uniform \"{}[{}]\" does not exist!  "
			"Program: {}", HN_ToStr(uniform), index, HN_ToStr(activeShaderName));
		EP_DEBUGBREAK();
		return;
	}
}

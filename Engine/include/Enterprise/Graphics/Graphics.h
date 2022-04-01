#pragma once
#include <stack>
#include <deque>
#include <glm/glm.hpp>
#include "Enterprise/Core.h"

enum class TextureFilter
{
	Nearest, Linear //, Anisotropic
};
enum class MipmapMode
{
	None, Nearest, Linear
};

enum class ShaderDataType
{
	none = 0,
	Float, Vec2, Vec3, Vec4,
	Int, UInt,
	Mat3, Mat4
};

enum class ImageFormat
{
	// Color
	RGB8,
	RGB16F,
	RGB32F,

	// Depth only
	Depth16,
	Depth24,
	Depth32,
	Depth32F,

	// Depth and Stencil
	Depth24Stencil8
};

enum class BlendMode
{
	Opaque, // f(s, d) = s
	Translucent, // f(s, d) = s * a + d * 1-a
	Additive, // f(s, d) = s + d
	Multiply // f(s, d) = s * d
};


namespace Enterprise
{

/// Enterprise's Graphics system.
class Graphics
{
public:

	// Shaders

	/// Compile and link a shader program from a string.
	/// @param src The source code of the shader program.
	/// @return True if successful, false if unsuccessful.
	/// @remarks If a shader program of the same name has been compiled previously, recompilation is skipped, and the 
	/// reference count to the existing shader is increased.
	EP_API static bool CompileShaderSrc(const std::string& src);
	/// Load, compile and link a shader program from a source file.
	/// @param path Path to the shader program source file.
	/// @return True if successful, false if unsuccessful.
	/// @remarks If a shader program of the same name has been compiled previously, recompilation is skipped, and the 
	/// reference count to the existing shader is increased.
	EP_API static bool LoadShaderFile(std::string path);
	/// Decrement the reference count for a shader program.
	/// @param shader The HashName of the loaded shader program.
	/// @remarks If the reference count for a shader program reaches 0, it is deleted.
	EP_API static void DeleteShader(HashName shader);

	/// Enable a shader option.
	/// @param option The HashName of the shader option.
	/// @param updateShader If @c true, the currently bound shader will switch to the given variant.
	/// @note The shader option is not used until the next call to Graphics::BindShader() or 
	/// Graphics::BindFallbackShader().
	EP_API static void EnableShaderOption(HashName option, bool updateShader = false);
	/// Disable a shader option.
	/// @param option The HashName of the option.
	/// @param updateShader If @c true, the currently bound shader will switch to the given variant.
	/// @note The shader option is not cleared until the next call to Graphics::BindShader() or 
	/// Graphics::BindFallbackShader().
	EP_API static void DisableShaderOption(HashName option, bool updateShader = false);
	/// Check whether a shader program is compatible with the active shader options.
	/// @param shader The HashName of the shader program to check.
	/// @return True if the shader program has a variant compiled that matches the currently active shader options.
	EP_API static bool isShaderViable(HashName shader);

	/// Make a shader program the fallback program.
	/// @remarks The fallback shader program is used when BindShader() fails.
	/// @param shader The HashName of the loaded shader program.
	/// @param options If the fallback shader does not have a variant compatible with the active shader options, the 
	/// options to use instead.
	/// @return True if successful, false if unsuccessful.
	EP_API static void SetFallbackShader(HashName shader, std::set<HashName> options);
	/// Remove the fallback shader program.
	EP_API static void UnsetFallbackShader();

	/// Make a shader program the active program.
	/// @param shader The HashName of the shader program.
	/// @remarks Shader binding will fail if no shader has been compiled with the given HashName, or if no variant 
	/// exists that supports the active shader options. If binding fails, the fallback shader will be used, if one has 
	/// been set.
	EP_API static void BindShader(HashName shader);


	// Uniform Buffers

	typedef unsigned int UniformBufferHandle;

	/// Create a uniform buffer.
	/// @param name The HashName of the uniform block the buffer will be bound to.
	/// @param size The size of the buffer in bytes.
	/// @param data Pointer to the data with which to initialize the buffer.  @c nullptr will leave the buffer uninitialized.
	/// @param dynamic Set to true if the buffer will be updated regularly.
	/// @return The handle of the created uniform buffer.
	/// @note The first uniform buffer with a given @c name is bound automatically upon creation.
	EP_API static UniformBufferHandle CreateUniformBuffer(HashName name, size_t size, void* data = nullptr, bool dynamic = true);
	/// Delete a uniform buffer.
	/// @param buffer The handle of the uniform buffer.
	/// @note If the buffer is currently bound, it will be unbound automatically.
	EP_API static void DeleteUniformBuffer(UniformBufferHandle buffer);

	/// Update the contents of a uniform buffer.
	/// @param buffer The handle of the uniform buffer.
	/// @param data Pointer to the data to copy into the buffer.
	/// @note The number of bytes copied is equal to the @c size value passed to Graphics::CreateUniformBuffer() when 
	/// @c buffer was created.
	/// @note The data must be in @c std140 format.
	EP_API static void SetUniformBufferData(UniformBufferHandle buffer, void* data);
	/// Update the contents of a uniform buffer.
	/// @param buffer The handle of the uniform buffer.
	/// @param data Pointer to the data to copy into the buffer.
	/// @param offset The offset into the buffer where data replacement will begin, measured in bytes.
	/// @param count The number of bytes to copy into the uniform buffer.
	/// @note The data must be in @c std140 format.
	EP_API static void SetUniformBufferData(UniformBufferHandle buffer, void* data, size_t offset, size_t count);
	/// Make a uniform buffer the active buffer.
	/// @param buffer The handle of the uniform buffer.
	EP_API static void BindUniformBuffer(UniformBufferHandle buffer);


	// Textures

	typedef unsigned int TextureHandle;

	/// Load a texture from an image file.
	/// @param path Path to the image file to load.
	/// @param minFilter The filtering function to use when the texture needs to be minified.
	/// @param magFilter The filtering function to use when the texture needs to be magnified.
	/// @param mipmapMode Whether and how mipmaps are used in sampling.
	/// @return The handle of the loaded texture.
	/// @note At present, only PNG, BMP, and JPEG formats are supported.
	/// @note If either the width or height of the image is 1, it is loaded into GPU memory as a 1D texture.
	EP_API static TextureHandle LoadTexture(std::string path,
									 TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode);
	/// Load a cubemap texture from an image file.
	/// @param path Path to the image file to load.
	/// @param minFilter The filtering function to use when the texture needs to be minified.
	/// @param magFilter The filtering function to use when the texture needs to be magnified.
	/// @param mipmapMode Whether and how mipmaps are used in sampling.
	/// @return The handle of the loaded texture.
	/// @note At present, only PNG, BMP, and JPEG formats are supported.
	/// @note The type of mapping used is automatically detected from the image file's aspect ratio.
	EP_API static TextureHandle LoadCubemapTexture(std::string path,
											TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode);
	/// Load a 3D texture from an image file.
	/// @param path Path to the image file to load.
	/// @param minFilter The filtering function to use when the texture needs to be minified.
	/// @param magFilter The filtering function to use when the texture needs to be magnified.
	/// @param mipmapMode Whether and how mipmaps are used in sampling.
	/// @param columns The number of columns to spit the image into.
	/// @param rows The number of rows to split the image into.
	/// @param depth The number of z-slices to extract from this image.
	/// @return The handle of the loaded texture.
	/// @note At present, only PNG, BMP, and JPEG formats are supported.
	/// @remarks During load, the image is partitioned into a grid that is @c columns wide and @c rows tall.
	/// The cells are loaded in left-to-right, top-to-bottom order as "slices" of the z direction going from @c 0.0f z to
	/// @c 1.0f z (normalized).  If @c depth is specified, only the first @c depth cells are loaded into the texture.
	EP_API static TextureHandle Load3DTexture(std::string path,
									   TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode,
									   size_t columns, size_t rows, size_t depth = 0);

	/// Load a texture array from multiple image files.
	/// @param paths Ordered list of the paths to the image files to load into the array.
	/// @param minFilter The filtering function to use when the textures need to be minified.
	/// @param magFilter The filtering function to use when the textures need to be magnified.
	/// @param mipmapMode Whether and how mipmaps are used in sampling.
	/// @return The handle of the loaded texture array.
	/// @note At present, only PNG, BMP, and JPEG formats are supported.
	/// @note All images in @c paths must have exactly the same dimensions.
	/// @note If either the width or height of the images in the array equal 1, the array will be loaded into GPU memory
	/// as a 1D texture array.
	EP_API static TextureHandle LoadTextureArray(std::initializer_list<std::string> paths,
										  TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode);
	/// Load a texture array from a single image file.
	/// @param path Path to the image file to load.
	/// @param minFilter The filtering function to use when the textures need to be minified.
	/// @param magFilter The filtering function to use when the textures need to be magnified.
	/// @param mipmapMode Whether and how mipmaps are used in sampling.
	/// @param columns The number of columns to spit the image into.
	/// @param rows The number of rows to split the image into.
	/// @param count The number of textures to extract from this image.
	/// @return The handle of the loaded texture array.
	/// @note At present, only PNG, BMP, and JPEG formats are supported.
	/// @note If the image height divided by @c rows is equal to 1, the array is loaded into GPU memory as a 1D texture array.
	/// @remarks During load, the image is partitioned into a grid that is @c columns wide and @c rows tall.
	/// The cells are loaded in left-to-right, top-to-bottom order as consecutive textures in the array.
	/// If @c count is specified, only the first @c count cells are loaded into the array.
	EP_API static TextureHandle LoadTextureArray(std::string path,
										  TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode,
										  size_t columns, size_t rows, size_t count = 0);
	/// Load a cubemap texture array from multiple image files.
	/// @param paths Ordered list of the paths to the image files to load into the array.
	/// @param minFilter The filtering function to use when the textures need to be minified.
	/// @param magFilter The filtering function to use when the textures need to be magnified.
	/// @param mipmapMode Whether and how mipmaps are used in sampling.
	/// @return The handle of the loaded texture array.
	/// @note At present, only PNG, BMP, and JPEG formats are supported.
	/// @note The type of mapping used is automatically detected from the image file's aspect ratio.
	/// @note All images in @c paths must have exactly the same dimensions.
	EP_API static TextureHandle LoadCubemapTextureArray(std::initializer_list<std::string> paths,
												 TextureFilter minFilter, TextureFilter magFilter, MipmapMode mipmapMode);

	/// Load a TrueType font file and generate a texture atlas for text rendering.
	/// @param path Path to the TTF file to load.
	/// @param emSizeMin The minimum font size the texture must have.
	/// @return The handle of the generated texture atlas.
	/// @remarks After loading the font, the atlas's TextureHandle can be passed into the "Graphics::GetFont..."
	/// functions for font metric retrieval.
	EP_API static TextureHandle LoadFontFile(const std::string& path, float emSizeMin = 32.0f);

	/// Get the UV bounds of a specific character in a font's texture atlas.
	/// @param atlas The handle of the font's texture atlas.
	/// @param unicodeChar The character for which to look up uv bounds.
	/// @param[out] out_l Variable to receive the x-coordinate of the left edge of the glyph in the texture atlas.
	/// @param[out] out_r Variable to receive the x-coordinate of the right edge of the glyph in the texture atlas.
	/// @param[out] out_b Variable to receive the y-coordinate of the bottom edge of the glyph in the texture atlas.
	/// @param[out] out_t Variable to receive the y-coordinate of the top edge of the glyph in the texture atlas.
	EP_API static void GetFontCharUVBounds(TextureHandle atlas, uint32_t unicodeChar,
									float& out_l, float& out_r, float& out_b, float& out_t);
	/// Get the bounds of the quad needed to render a specific character in a font's texture atlas.
	/// @param atlas The handle of the font's texture atlas.
	/// @param unicodeChar The character for which to look up quad bounds.
	/// @param[out] out_l Variable to receive the x-coordinate of the left edge of the glyph relative to the cursor origin.
	/// @param[out] out_r Variable to receive the x-coordinate of the right edge of the glyph relative to the cursor origin.
	/// @param[out] out_b Variable to receive the y-coordinate of the bottom edge of the glyph relative to the cursor origin.
	/// @param[out] out_t Variable to receive the y-coordinate of the top edge of the glyph relative to the cursor origin.
	/// @param prevChar The character immediately preceding @c unicodeChar.  Setting this value will apply kerning.
	EP_API static void GetFontCharQuadBounds(TextureHandle atlas, uint32_t unicodeChar,
									  float& out_l, float& out_r, float& out_b, float& out_t, uint32_t prevChar = 0);
	/// Get the advance value associated with a specific character in a font.
	/// @param atlas The handle of the font's texture atlas.
	/// @param unicodeChar The character to look up.
	/// @return The cursor advance value associated with @c unicodeChar.
	EP_API static double GetFontCharAdvance(TextureHandle atlas, uint32_t unicodeChar);

	/// Get vertical alignment metrics for text written with a font.
	/// @param atlas The handle of the font's texture atlas.
	/// @param[out] out_lineHeight Variable to receive the distance between consecutive baselines, in ems.
	/// @param[out] out_ascenderHeight Variable to receive the distance between the baseline and the top of the tallest font glyph, in ems.
	/// @param[out] out_descenderHeight Variable to receive the distance between the baseline and the bottom of the lowest font glyph, in ems.
	EP_API static void GetFontVerticalMetrics(TextureHandle atlas,
									   double& out_lineHeight, double& out_ascenderHeight, double& out_descenderHeight);

	/// Get the HashName of the path to a loaded texture file.
	/// @param texture The handle of the texture to look up.
	/// @return The HashName of the path which was used to load this texture.
	EP_API static HashName GetTextureHashedPath(TextureHandle texture);

	/// Decrement the reference count for a texture.
	/// @param texture The handle of the texture.
	/// @note If the reference count for a texture reaches 0, it is deleted from GPU memory.
	EP_API static void DeleteTexture(TextureHandle texture);
	/// Bind a texture to a sampler uniform in a shader program.
	/// @param texture The handle of the texture.
	/// @param uniform The HashName of the sampler uniform to bind to.
	/// @param index If the sampler uniform is an array, the element of the array to bind to.
	EP_API static void BindTexture(TextureHandle texture, HashName uniform, unsigned int index = 0);


	// Global shader variables

	/// Add a camera to the camera stack without setting its properties.
	/// @note The camera added with this function will not update camera-related shader variables until its properties 
	/// are set with @c Graphics::SetCamera().
	EP_API static void PushCamera();
	/// Add a camera to the camera stack.
	/// @param cameraPos The position of the camera in world space.
	/// @param cameraRot The orientation of the camera, expressed as a quaternion.
	/// @param projectionMat The projection matrix for this camera.
	/// @note The global shader variables @c ep_matrix_mv and @c ep_matrix_mvp will not update with the new view and
	/// view-projection matrices until the next call to @c Graphics::SetModelMatrix().
	EP_API static void PushCamera(glm::vec3 cameraPos, glm::quat cameraRot, glm::mat4 projectionMat);
	/// Update the camera currently on top of the camera stack.
	/// @param cameraPos The position of the camera in world space.
	/// @param cameraRot The orientation of the camera, expressed as a quaternion.
	/// @param projectionMat The projection matrix for this camera.
	/// @note The global shader variables @c ep_matrix_mv and @c ep_matrix_mvp will not update with the new view and
	/// view-projection matrices until the next call to @c Graphics::SetModelMatrix().
	/// @note To use this function, you must first push a camera on the camera stack using @c Graphics::PushCamera().
	EP_API static void SetCamera(glm::vec3 cameraPos, glm::quat cameraRot, glm::mat4 projectionMat);
	/// Remove the topmost camera from the camera stack.
	EP_API static void PopCamera();

	/// Set the model matrix to be used in subsequent draw calls.
	/// @param modelMat The transformation matrix used to move vertices from model space to world space.
	EP_API static void SetModelMatrix(glm::mat4 modelMat);


	// Vertex array functions

	typedef unsigned int VertexArrayHandle;

	/// Create a vertex array.
	/// @param maxVertices The maximum number of vertices this array can hold.
	/// @param maxIndices The maximum number of indices this array can hold.  Set to @c 0 to use non-indexed rendering.
	/// @param vertexStride The distance, in bytes, between consecutive vertices in the vertex buffer.
	/// @param vertexLayout Description of the vertex data layout.  First: HashName of attribute. Second: Data Type,
	/// Third: Number of elements, Fourth: Offset of attribute in bytes
	/// @param vertexData Pointer to vertex buffer initialization data.  If @c nullptr, initialization is not performed.
	/// @param indexData Pointer to index buffer initialization data.  If @c nullptr, initialization is not performed.
	/// @param dynamicVertices Whether the vertex buffer should be rewriteable.
	/// @param dynamicIndices Whether the index buffer should be rewriteable.
	/// @note When this function is called with a non @c nullptr @c vertexData or @c indexData, the number of bytes copied
	/// is equal to the size of the recipient buffer.
	EP_API static VertexArrayHandle CreateVertexArray(size_t maxVertices, size_t maxIndices, size_t vertexStride,
											   std::initializer_list<std::tuple<HashName, ShaderDataType, size_t, size_t>> vertexLayout,
											   void* vertexData = nullptr, void* indexData = nullptr,
											   bool dynamicVertices = true, bool dynamicIndices = true);
	/// Delete a vertex array.
	/// @param array Handle of the vertex array.
	EP_API static void DeleteVertexArray(VertexArrayHandle array);


	/// Update the data in a vertex array's vertex buffer.
	/// @param array Handle of the vertex array.
	/// @param data Pointer to the source buffer.
	/// @param first Index of the first vertex to replace.
	/// @param count Number of vertices to copy.
	EP_API static void SetVertexData(VertexArrayHandle array, void* data, size_t first, size_t count);
	/// Update the data in a vertex array's index buffer.
	/// @param array Handle of the vertex array.
	/// @param data Pointer to the source buffer.
	/// @param first Index of the first index to replace.
	/// @param count Number of indices to copy.
	EP_API static void SetIndexData(VertexArrayHandle array, void* data, size_t first, size_t count);


	/// Render triangles from a vertex array.
	/// @param array Handle of the vertex array.
	/// @param count The number of elements to render.
	/// @param first The index of the first element to render.
	/// @note If the array has no index buffer, non-indexed rendering is performed.  In that case, @c count and @c first
	/// correspond to vertices, not elements.
	EP_API static void DrawTriangles(VertexArrayHandle array, size_t count = 0, size_t first = 0);
	/// Render a triangle strip from a vertex array.
	/// @param array Handle of the vertex array.
	/// @param count The number of elements to render.
	/// @param first The index of the first element to render.
	/// @note If the array has no index buffer, non-indexed rendering is performed.  In that case, @c count and @c first
	/// correspond to vertices, not elements.
	EP_API static void DrawTriangleStrip(VertexArrayHandle array, size_t count = 0, size_t first = 0);
	/// Render lines from a vertex array.
	/// @param array Handle of the vertex array.
	/// @param count The number of elements to render.
	/// @param first The index of the first element to render.
	/// @note If the array has no index buffer, non-indexed rendering is performed.  In that case, @c count and @c first
	/// correspond to vertices, not elements.
	EP_API static void DrawLines(VertexArrayHandle array, size_t count = 0, size_t first = 0);
	/// Render a line strip from a vertex array.
	/// @param array Handle of the vertex array.
	/// @param count The number of elements to render.
	/// @param first The index of the first element to render.
	/// @note If the array has no index buffer, non-indexed rendering is performed.  In that case, @c count and @c first
	/// correspond to vertices, not elements.
	EP_API static void DrawLineStrip(VertexArrayHandle array, size_t count = 0, size_t first = 0);
	/// Render points from a vertex array.
	/// @param array Handle of the vertex array.
	/// @param count The number of elements to render.
	/// @param first The index of the first element to render.
	/// @note If the array has no index buffer, non-indexed rendering is performed.  In that case, @c count and @c first
	/// correspond to vertices, not elements.
	EP_API static void DrawPoints(VertexArrayHandle array, size_t count = 0, size_t first = 0);


	// Framebuffers

	/// A description of a framebuffer attachment.
	struct FramebufferAttachmentSpec
	{
		ImageFormat format;
		TextureFilter minFilter;
		TextureFilter magFilter;
		bool makeTexture;
	};

	typedef unsigned int FramebufferHandle;

	/// Create a framebuffer.
	/// @param attachments A list of specifications for framebuffer attachments.
	/// @param width The width, in pixels, of the framebuffer.
	/// @param height The height, in pixels, of the framebuffer.
	/// @return Handle of the created framebuffer object.
	/// @note If width and height both equal @c 0, then the framebuffer will take on the dimensions of the game window, and
	/// automatically adjust to changes in window size.
	EP_API static FramebufferHandle CreateFramebuffer(std::initializer_list<FramebufferAttachmentSpec> attachments, int width = 0, int height = 0);
	/// Delete a framebuffer.
	/// @param fb Handle of the framebuffer.
	EP_API static void DeleteFramebuffer(FramebufferHandle fb);

	/// Push a framebuffer on top of the framebuffer stack.
	/// @param fb The handle of the framebuffer.
	EP_API static void PushFramebuffer(FramebufferHandle fb);
	/// Push a framebuffer on top of the framebuffer stack and immediately set a new viewport.
	/// @param fb The handle of the framebuffer.
	/// @param l The x coordinate, in [0, 1], of the left edge of the viewport.
	/// @param r The x coordinate, in [0, 1], of the right edge of the viewport.
	/// @param b The y coordinate, in [0, 1], of the bottom edge of the viewport.
	/// @param t The y coordinate, in [0, 1], of the top edge of the viewport.
	EP_API static void PushFramebuffer(FramebufferHandle fb, float l, float r, float b, float t);
	/// Remove the topmost framebuffer from the framebuffer stack.
	EP_API static void PopFramebuffer();

	/// Get the texture handle for a color buffer.
	/// @param fb Handle of the framebuffer.
	/// @param number The number of the color buffer.
	/// @return Handle of the texture used for the color attachment.  If the attachment is a render buffer or doesn't exist,
	/// returns @c 0.
	EP_API static TextureHandle GetColorTexture(FramebufferHandle fb, int number);
	/// Get the texture handle for a framebuffer's depth attachment.
	/// @param fb The handle of the framebuffer.
	/// @return Handle of the texture used for the depth attachment.  If the attachment is a render buffer or doesn't exist,
	/// returns @c 0.
	EP_API static TextureHandle GetDepthTexture(FramebufferHandle fb);
	/// Get the width, in pixels, of a framebuffer.
	/// @param fb The handle of the framebuffer.
	/// @return Width, in pixels, of the framebuffer.
	EP_API static int GetFramebufferWidth(FramebufferHandle fb);
	/// Get the height, in pixels, of a framebuffer.
	/// @param fb The handle of the framebuffer.
	/// @return Height, in pixels, of the framebuffer.
	EP_API static int GetFramebufferHeight(FramebufferHandle fb);
	/// Set a new size for a framebuffer.
	/// @param fb The handle of the framebuffer.
	/// @param width The new width in pixels.
	/// @param height The new height in pixels.
	EP_API static void ResizeFramebuffer(FramebufferHandle fb, int width, int height);

	/// Specify the region of the framebuffer in which to draw.
	/// @param l The x coordinate, in [0, 1], of the left edge of the viewport.
	/// @param r The x coordinate, in [0, 1], of the right edge of the viewport.
	/// @param b The y coordinate, in [0, 1], of the bottom edge of the viewport.
	/// @param t The y coordinate, in [0, 1], of the top edge of the viewport.
	EP_API static void SetViewport(float l, float r, float b, float t);

	/// Clear the current viewport of the current framebuffer to a flat color.
	/// @param color The color to clear to.
	EP_API static void ClearCurrentViewport(glm::vec4 color = { 0,0,0,0 });

	EP_API static void SetBlendMode(BlendMode mode);
	EP_API static void SetDepthTest(bool enable);
	EP_API static void SetBackfaceCulling(bool enable);
	

private:

	// Shader stuff

	static HashName selectedShaderName;
	static HashName activeShaderName;
	static HashName fallbackShaderName;
	static std::set<HashName> selectedShaderOptions;
	static std::set<HashName> activeShaderOptions;
	static std::set<HashName> fallbackShaderOptions;

	static std::map<HashName, std::map<std::set<HashName>, std::map<HashName, unsigned int>>> shaderAttribLocations;
	static std::map<HashName, std::map<std::set<HashName>, std::map<HashName, int>>> shaderAttribArrayLengths;
	static std::map<HashName, std::map<std::set<HashName>, std::map<HashName, ShaderDataType>>> shaderAttribTypes;

	// Uniform buffer stuff

	static int maxUniformBufferSize;
	static int maxUniformBindingPoints;
	static std::map<Graphics::UniformBufferHandle, size_t> uniformBufferSizes;
	static std::map<HashName, std::map<HashName, size_t>> uniformBlockSizes; // Key 1: Program name Key 2: Block name

	// Texture stuff

	static int maxTextureSlots;
	static TextureHandle* textureInSlot;
	static bool* isTextureSlotUsedThisDraw;
	static int lastBoundTextureSlot;
	static std::map<TextureHandle, int> currentSlotOfTexture;

	// Global uniform buffers

	static UniformBufferHandle perCameraGlobalUB;
	struct perCameraGlobalUBStruct
	{
		glm::mat4 ep_matrix_v;
		glm::mat4 ep_matrix_p;
		glm::mat4 ep_matrix_vp;
		glm::vec3 ep_camera_worldpos;

		perCameraGlobalUBStruct(glm::mat4 v, glm::mat4 p, glm::mat4 vp, glm::vec3 campos)
								: ep_matrix_v(v), ep_matrix_p(p), ep_matrix_vp(vp), ep_camera_worldpos(campos) {}
	};
	static std::stack<perCameraGlobalUBStruct> perCameraGlobalUBDataStack;

	static UniformBufferHandle perDrawGlobalUB;
	static struct perDrawGlobalUBStruct
	{
		glm::mat4 ep_matrix_m;
		glm::mat4 ep_matrix_mv;
		glm::mat4 ep_matrix_mvp;
	} perDrawGlobalUBData;

	// Drawing stuff

	static void BindVertexArrayForDraw(VertexArrayHandle& array, size_t& count, size_t& first);

	// Framebuffer stuff

	static std::map<Graphics::FramebufferHandle, int> fbWidths;
	static std::map<Graphics::FramebufferHandle, int> fbHeights;
	static std::deque<glm::vec4> fbViewports;

	friend class Application;

	static void Init();
	static void PreDraw();
	static void Cleanup();
};

}

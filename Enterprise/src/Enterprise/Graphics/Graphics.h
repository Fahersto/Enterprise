#pragma once
#include "EP_PCH.h"
#include "Core.h"

#include "Math.h"

namespace Enterprise
{

namespace Constants
{
extern const unsigned int QuadBatch_MaxQuads;
}

/// Enterprise's Graphics system.
class Graphics
{
public:
	/// Handle for a compiled vertex shader.
	typedef unsigned int VShaderRef;
	/// Handle for a compiled pixel shader.
	typedef unsigned int PShaderRef;
	/// Handle for a shader program.
	typedef unsigned int ProgramRef;
	/// Handle for a loaded texture.
	typedef unsigned int TextureRef;
	/// Handle for a vertex array.
	typedef unsigned int ArrayRef;

	/// A data type accepted by a shader.
	enum class ShaderDataType
	{
		none = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Mat3, Mat4
	};

	// Shader functions

	/// Compile and load a vertex shader from a string.
	/// @param src The source code of the vertex shader.
	/// @return Handle of the created shader.  If compilation failed, returns 0.
	static VShaderRef LoadVertexShaderFromString(std::string src);
	/// Compile and load a vertex shader from a source file.
	/// @param path Path to the vertex shader source file.
	/// @return Handle of the created shader.  If compilation failed, returns 0.
	static VShaderRef LoadVertexShader(std::string path);
	/// Delete a vertex shader.
	/// @param shader Handle of the vertex shader.
	/// @note It is safe to delete a shader while it is still linked to an active program.
	static void DeleteVertexShader(VShaderRef shader);

	/// Compile and load a pixel shader from a string.
	/// @param src The source code of the pixel shader.
	/// @return Handle of the created shader.  If compilation failed, returns 0.
	static PShaderRef LoadPixelShaderFromString(std::string src);
	/// Compile and load a pixel shader from a source file.
	/// @param path Path to the pixel shader source file.
	/// @return Handle of the created shader.  If compilation failed, returns 0.
	static PShaderRef LoadPixelShader(std::string path);
	/// Delete a pixel shader.
	/// @param shader Handle of the pixel shader.
	/// @note It is safe to delete a shader while it is still linked to an active program.
	static void DeletePixelShader(PShaderRef shader);

	/// Link a vertex and pixel shader into a shader program.
	/// @param vShader Handle of the vertex shader.
	/// @param pShader Handle of the pixel shader.
	/// @return Handle of the linked program.  If linking fails, returns 0.
	static ProgramRef LinkShaders(VShaderRef vShader, PShaderRef pShader);
	/// Make a shader program the active program.
	/// @param program Handle of the shader program.
	static void BindProgram(ProgramRef program);
	/// Delete a shader program.
	/// @param program Handle of the shader program.
	static void DeleteProgram(ProgramRef program);

	// Uniform functions

	/// Set the current transform matrix.
	/// @param transform The transform matrix to use in the next draw call.
	/// @note This is equivalent to setting the "transform" uniform.
	static void SetTransform(Math::Mat4 transform);
	/// Set the current projection matrix.
	/// @param projection The projection matrix to use in the next draw call.
	/// @note This is equivalent to setting the "projection" uniform.
	static void SetProjection(Math::Mat4 projection);

	/// Set the value of a Float uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, float value);
	/// Set the value of a Float2 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	static void SetUniform(HashName uniform, float value1, float value2);
	/// Set the value of a Float3 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	/// @param value3 The third value.
	static void SetUniform(HashName uniform, float value1, float value2, float value3);
	/// Set the value of a Float4 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	/// @param value3 The third value.
	/// @param value4 The fourth value.
	static void SetUniform(HashName uniform, float value1, float value2, float value3, float value4);

	/// Set the value of a Float2 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, Math::Vec2 value);
	/// Set the value of a Float3 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, Math::Vec3 value);
	/// Set the value of a Float4 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, Math::Vec4 value);

	/// Set the value of an Int uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, int value);
	/// Set the value of an Int2 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	static void SetUniform(HashName uniform, int value1, int value2);
	/// Set the value of an Int3 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	/// @param value3 The third value.
	static void SetUniform(HashName uniform, int value1, int value2, int value3);
	/// Set the value of an Int4 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	/// @param value3 The third value.
	/// @param value4 The fourth value.
	static void SetUniform(HashName uniform, int value1, int value2, int value3, int value4);

	/// Set the value of a UInt uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, unsigned int value);
	/// Set the value of a UInt2 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	static void SetUniform(HashName uniform, unsigned int value1, unsigned int value2);
	/// Set the value of a UInt3 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	/// @param value3 The third value.
	static void SetUniform(HashName uniform, unsigned int value1, unsigned int value2, unsigned int value3);
	/// Set the value of a UInt4 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value1 The first value.
	/// @param value2 The second value.
	/// @param value3 The third value.
	/// @param value4 The fourth value.
	static void SetUniform(HashName uniform, unsigned int value1, unsigned int value2, unsigned int value3, unsigned int value4);

	/// Set the value of a Mat3 uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, Math::Mat3 value);
	/// Set the value of a Mat4uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, Math::Mat4 value);

	static void SetUniformArray(HashName uniform, unsigned int count, ShaderDataType type, void* src);

	// Texture functions

	/// Load a texture from an image file.
	/// @param path Path to the texture file.
	/// @return Handle of the loaded texture.
	static TextureRef LoadTexture(std::string path);
	/// Bind a texture to an active texture slot.
	/// @param texture Handle of the texture.
	/// @param slot The texture slot to bind to.
	/// @note To unbind a slot, use @c texture @c = @c 0.
	static void BindTexture(TextureRef texture, unsigned int slot = 0);
	/// Delete a texture.
	/// @param texture Handle of the texture.
	static void DeleteTexture(TextureRef texture);

	// Vertex array functions

	/// Create a vertex array.
	/// @param dynamicVertices Whether the vertex buffer should use easily-rewriteable memory.
	/// @param dynamicIndices Whether the index buffer should use easily-rewriteable memory.
	/// @param maxVertices The maximum number of vertices this array can hold.
	/// @param maxTriangles The maximum number of triangles this array can describe.
	/// @param layout The layout of vertex data in the vertex buffer.
	/// @note The vertex array will automatically be bound when it is created.
	/// @note The array's internal index buffer will have a size equal to @c maxTriangles @c * @c 3.
	static ArrayRef CreateVertexArray(bool dynamicVertices, bool dynamicIndices,
									  size_t maxVertices, size_t maxTriangles,
									  std::initializer_list<std::pair<HashName, ShaderDataType>> layout);
	/// Delete a vertex array.
	/// @param array Handle of the vertex array.
	static void DeleteVertexArray(ArrayRef array);

	/// Update the data in a vertex array's vertex buffer.
	/// @param array Handle of the vertex array.
	/// @param src Pointer to the source buffer.
	/// @param first Index of the first vertex to replace.
	/// @param count Number of vertices to copy from the source buffer.
	static void SetVertexData(ArrayRef array, void* src, unsigned int first, unsigned int count);
	/// Update the data in a vertex array's index buffer.
	/// @param array Handle of the vertex array.
	/// @param src Pointer to the source buffer.
	/// @param first Index of the first index to replace.
	/// @param count Number of indices to copy from the source buffer.
	static void SetIndexData(ArrayRef array, unsigned int* src, unsigned int first, unsigned int count);

	/// Draw a vertex array.
	/// @param array Handle of the array.
	static void DrawArray(ArrayRef array);
	/// Draw a vertex array.
	/// @param array Handle of the array.
	/// @param triangleCount Number of triangles to draw.
	static void DrawArray(ArrayRef array, unsigned int triangleCount);

	// Viewport functions

	/// Set a subsection of the game window as the render target.
	/// @param pos The coordinates, in pixel space, of the upper-left corner of the viewport.
	/// @param size The dimensions, in pixel space, of the viewport.
	/// @note To render to the entire game window, call this function with @c pos @c = @c {0, @c  0} and @c size equal to the window size.
	static void TargetViewportForRender(Math::Vec2 pos, Math::Vec2 size);
	/// Set a texture as the render target.
	/// @param texture Handle of the texture.
	static void TargetTextureForRender(TextureRef texture);
	/// Clears the current render target.
	static void ClearRenderTarget();


	/// 2D batch rendering functions.
	class QuadBatch
	{
	public:
		/// Start a new quad batch.
		static void Begin();

		/// Add a quad to the current batch.
		/// @param scale Dimensions of the quad.
		/// @param translation Position of the quad in world space.
		/// @param uv_lowerleft UV coordinates for the lower-left corner of the quad.
		/// @param uv_topright UV coordinates for the lower-left corner of the quad.
		/// @param textures List of textures used by this quad.
		static void AddQuad(Math::Vec2 scale, Math::Vec3 translation,
							Math::Vec2 uv_lowerleft, Math::Vec2 uv_topright,
							std::initializer_list<Graphics::TextureRef> textures);

		/// Add a rotated quad to the current batch.
		/// @param origin Coordinates in [(0, 0), (1, 1)] of quad center.
		/// @param scale 2D scale factor.
		/// @param roll Rotation in degrees about the Z axis.
		/// @param pitch Rotation in degrees about the X axis.
		/// @param yaw Rotation in degrees about the Y axis.
		/// @param translation Position of the quad in world space.
		/// @param uv_lowerleft UV coordinates for the lower-left corner of the quad.
		/// @param uv_topright UV coordinates for the upper-right corner of the quad.
		/// @param textures List of textures used by this quad.
		static void AddRotatedQuad(Math::Vec2 origin,
								   Math::Vec2 scale, float roll, float pitch, float yaw, Math::Vec3 translation,
								   Math::Vec2 uv_lowerleft, Math::Vec2 uv_topright,
								   std::initializer_list<Graphics::TextureRef> textures);

		// TODO: Create overloads
		static void QuadAttribute(HashName attribute, float value);
		static void VertexAttribute(uint_fast8_t vertex, HashName attribute, float value);

		/// End the current quad batch and draw it.
		static void End();
	};

	//class MeshBatch
	//{
	//};

private:
	static ProgramRef _activeProgram; // Handle of the currently active shader program.
	static Graphics::ArrayRef _activeArray; // Handle of the currently active vertex array (0 if none are active).
	static uint64_t _enabledAttributes; // Bit field representing the enable/disable status of OpenGL vertex attributes.

	static std::unordered_map<ProgramRef, std::unordered_map<HashName, unsigned int>> _shaderAttributeIndices;
	static std::unordered_map<ProgramRef, std::unordered_map<HashName, std::tuple<unsigned int, ShaderDataType, uint64_t>>> _quadBatchVertexInfo; // index, type, offset
	static std::unordered_map<ProgramRef, unsigned int> _quadBatchVertexStrides;

	static unsigned int _quadbatch_vbo, _quadbatch_ibo;
	struct QuadBatchDefaultVertex
	{
		Math::Vec3 ep_pos;
		Math::Vec2 ep_uv;
		int ep_tex;
	};
	static int _maxTextureSlots;
	static int* _textureSlots;

	friend class Application;

	static void Init();
	static void Update();
	static void Cleanup();
};

}

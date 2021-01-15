#pragma once
#include "EP_PCH.h"
#include "Core.h"

#include "Math.h"

namespace Enterprise
{

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
	static void SetTransform(Math::Matrix4 transform);
	/// Set the current projection matrix.
	/// @param projection The projection matrix to use in the next draw call.
	/// @note This is equivalent to setting the "projection" uniform.
	static void SetProjection(Math::Matrix4 projection);

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
	static void SetUniform(HashName uniform, Math::Matrix3 value);
	/// Set the value of a Mat4uniform.
	/// @param uniform The HashName of the uniform.
	/// @param value The value.
	static void SetUniform(HashName uniform, Math::Matrix4 value);

	// Texture functions

	/// Load a texture from an image file.
	/// @param path Path to the texture file.
	/// @return Handle of the loaded texture.
	static TextureRef LoadTexture(std::string path);
	/// Bind a texture to an active texture slot.
	/// @param texture Handle of the texture.
	/// @param slot The texture slot to bind to.
	/// @note To unbind a slot, use @c texture @c = @c 0.
	static void BindTexture(TextureRef texture, unsigned int slot);
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


	/// 2D rendering functions.
	class R2D
	{
	public:
		/// Start a new 2D quad batch.
		static void BeginBatch();

		/// Add a 2D quad to the current batch.
		/// @param pos Position of the quad in world space.
		/// @param size Dimensions of the quad in world space.
		/// @param color Color to tint the quad.
		/// @param tex Handle of the texture to use.
		/// @param UV_TopLeft UV coordinates for the upper-left corner of the quad.
		/// @param UV_BottomRight UV coordinates for the lower-right corner of the quad.
		static void BatchQuad(Math::Vec2 pos, Math::Vec2 size,
							  Math::Vec4 color, TextureRef tex, Math::Vec2 UV_TopLeft, Math::Vec2 UV_BottomRight);

		/// Add a rotated 2D quad to the current batch.
		/// @param pos Position of the quad in world space.
		/// @param size Dimensions of the quad in world space.
		/// @param origin Position of the quad's center of rotation in normalized coordinates.
		/// @param rot Rotation in degrees.
		/// @param color Color to tint the quad.
		/// @param tex Handle of the texture to use.
		/// @param UV_TopLeft UV coordinates for the upper-left corner of the quad.
		/// @param UV_BottomRight UV coordinates for the lower-right corner of the quad.
		/// @note @c origin is also used as the center of scaling.
		static void BatchRotatedQuad(Math::Vec2 pos, Math::Vec2 size,
									 Math::Vec2 origin, float rot,
									 Math::Vec4 color, TextureRef tex, Math::Vec2 UV_TopLeft, Math::Vec2 UV_BottomRight);

		/// End the current 2D quad batch and draw it.
		static void EndBatch();
	};

private:

	/// Handle of the currently active shader program.
	static ProgramRef _activeProgram;

	/// Table of each shader program's vertex attribute indices.
	static std::unordered_map<ProgramRef, std::unordered_map<HashName, unsigned int>> _shaderAttributeIndices;

	friend class Application;

	static void Init();
	static void Update();
	static void Cleanup();
};

}

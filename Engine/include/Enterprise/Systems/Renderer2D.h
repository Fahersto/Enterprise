#pragma once
#include "Enterprise/Core.h"
#include "Enterprise/Graphics/Graphics.h"
#include "Enterprise/SceneManager/SceneManager.h"

// Needed to avoid Win32 name collision
#ifdef DrawText
#undef DrawText
#endif

enum class TextAlignment
{
	TopLeft, TopCenter, TopRight,
	MidLeft, MidCenter, MidRight,
	BottomLeft, BottomCenter, BottomRight
};

namespace Enterprise
{

/// A basic 2D renderer.
class Renderer2D
{
public:

	/// Start a new sprite batch.
	EP_API static void BeginBatch();
	/// Render a new sprite as part of a sprite batch.
	/// @param texture The handle of the sprite's associated texture.
	/// @param uv_l The x-coordinate of the left edge of the texture sample region.
	/// @param uv_r The x-coordinate of the right edge of the texture sample region.
	/// @param uv_b The y-coordinate of the bottom edge of the texture sample region.
	/// @param uv_t The y-coordinate of the top edge of the texture sample region.
	/// @param position The position of the sprite in world coordinates.
	/// @param rotation The orientation of the sprite in world coordinates.
	/// @param scale The scale of the sprite in world units.
	EP_API static void DrawSprite(Graphics::TextureHandle texture,
		float uv_l, float uv_r, float uv_b, float uv_t,
		glm::vec3 position, glm::quat rotation, glm::vec2 scale);
	/// End a sprite batch and draw it to the screen.
	EP_API static void EndBatch();

	// /// Render a line of text.
	// /// @param text The string of text to render.
	// /// @param atlas The handle of a loaded font's texture atlas.
	// /// @param position The position of the text in world space.
	// /// @param rotation The orientation of the text in world space.
	// /// @param size The font size in world space units.
	// /// @param alignment The alignment of the rendered text.
	// EP_API static void DrawText(std::string text, Graphics::TextureHandle atlas,
	// 	glm::vec3 position, glm::quat rotation, float size,
	// 	TextAlignment alignment = TextAlignment::TopLeft);

	/// Delete any sprite component associated with an entity.
	/// @param entity The ID of the entity to delete components from.
	EP_API static void DeleteSpriteComponent(EntityID entity);
	/// Get a list of entities with a sprite component attached.
	/// @return A vector of EntityIDs currently associated with a sprite component.
	EP_API static std::vector<EntityID> GetEntitiesWithSpriteComponents();
	/// Write out sprite component data for an entity in YAML format.
	/// @param entity The ID of the entity to serialize sprite components for.
	/// @param yamlOut A YAML node to receive the serialized sprite component data.
	/// @return @c true if serialization was successful.
	EP_API static bool SerializeSpriteComponent(EntityID entity, YAML::Node& yamlOut);
	/// Instantiate a sprite component serialized in YAML format.
	/// @param entity The ID of the entity to attach the loaded sprite component to.
	/// @param yamlIn A YAML node containing the serialized sprite component data.
	/// @return @c true if deserialization was successful.
	EP_API static bool DeserializeSpriteComponent(EntityID entity, const YAML::Node& yamlIn);

	/// Initialize Renderer2D.
	/// @param maxSpriteComponents The maximum number of sprite components to support.
	EP_API static void Init(size_t maxSpriteComponents);

private:
	struct SpriteComponent
	{
		EntityID id;
		Graphics::TextureHandle tex;
		float uv_bounds[4]; // l, r, b, t
	};
	static std::vector<SpriteComponent> spriteComponents;
	static std::vector<TextureFilter> minFilters;
	static std::vector<TextureFilter> magFilters;
	static std::vector<MipmapMode> mipModes;

	struct VertexData
	{
		glm::vec3 ep_position;
		glm::vec2 in_uv;
		int in_tex;
	};
	static std::vector<VertexData> quadVertices;
	static Graphics::VertexArrayHandle quadVAH;

	static void SceneDraw();
	static void DrawSpriteComponents();
};

}

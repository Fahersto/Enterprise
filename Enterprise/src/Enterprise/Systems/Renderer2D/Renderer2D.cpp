#include "EP_PCH.h"
#include "../Renderer2D.h"

using Enterprise::Renderer2D;
using Enterprise::Graphics;

std::vector<Renderer2D::VertexData> Renderer2D::quadVertices;
Graphics::VertexArrayHandle Renderer2D::quadVAH;

void Renderer2D::Init(size_t maxSpriteComponents)
{
	spriteComponents.resize(maxSpriteComponents);
	minFilters.resize(maxSpriteComponents);
	magFilters.resize(maxSpriteComponents);
	mipModes.resize(maxSpriteComponents);
	quadVertices.resize(4 * maxSpriteComponents);

	SceneManager::RegisterSceneDrawFn(SceneDraw);
	SceneManager::RegisterComponentType(
		HN("Sprite"),
		DeleteSpriteComponent,
		GetEntitiesWithSpriteComponents,
		SerializeSpriteComponent,
		DeserializeSpriteComponent);

	unsigned int* indexInitBuffer = new unsigned int[6 * maxSpriteComponents];
	for (unsigned int i = 0; i < maxSpriteComponents; i++)
	{
		indexInitBuffer[i * 6] = i * 4;
		indexInitBuffer[i * 6 + 1] = i * 4 + 1;
		indexInitBuffer[i * 6 + 2] = i * 4 + 2;
		indexInitBuffer[i * 6 + 3] = i * 4 + 2;
		indexInitBuffer[i * 6 + 4] = i * 4 + 3;
		indexInitBuffer[i * 6 + 5] = i * 4;
	}

	quadVAH = Graphics::CreateVertexArray(4 * maxSpriteComponents, 6 * maxSpriteComponents, sizeof(VertexData),
		{
			{ HN("ep_position"), ShaderDataType::Vec3, 1, offsetof(VertexData, ep_position) },
			{ HN("in_uv"), ShaderDataType::Vec2, 1, offsetof(VertexData, in_uv) },
			{ HN("in_tex"), ShaderDataType::Int, 1, offsetof(VertexData, in_tex) }
		}, nullptr, indexInitBuffer, true, false);

	delete[] indexInitBuffer;
}

void Renderer2D::SceneDraw()
{
	DrawSpriteComponents();
}

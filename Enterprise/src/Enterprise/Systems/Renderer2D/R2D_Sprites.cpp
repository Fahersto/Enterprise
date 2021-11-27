#include "EP_PCH.h"
#include "../Renderer2D.h"

using namespace Enterprise;

std::vector<Renderer2D::SpriteComponent> Renderer2D::spriteComponents;
std::vector<TextureFilter> Renderer2D::minFilters;
std::vector<TextureFilter> Renderer2D::magFilters;
std::vector<MipmapMode> Renderer2D::mipModes;
static size_t spriteComponentCount = 0;
static std::map<EntityID, size_t> spriteComponentIndices;

static Graphics::TextureHandle samplerSlots[16];
static size_t numOfAssignedSamplerSlots = 0;
static size_t spritesToPush = 0;
static bool inBatch = false;

void Renderer2D::BeginBatch()
{
	if (inBatch)
	{
		EP_WARN("Renderer2D::BeginBatch(): Already assembling a sprite batch!  Starting new batch.");
		EndBatch();
	}

	numOfAssignedSamplerSlots = 0;
	spritesToPush = 0;
	Graphics::SetModelMatrix(glm::mat4(1.0f));
	Graphics::EnableShaderOption(HN("BATCHED_TEXTURES"), true);
	inBatch = true;
}

void Renderer2D::DrawSprite(Graphics::TextureHandle texture,
	float uv_l, float uv_r, float uv_b, float uv_t,
	glm::vec3 position, glm::quat rotation, glm::vec2 scale)
{
	if (!inBatch)
	{
		EP_WARN("Renderer2D::DrawSprite(): Attempted to draw sprite without first calling Renderer2D::BeginBatch()!  "
			"Automatically starting new batch.  Texture: {}", HN_ToStr(Graphics::GetTextureHashedPath(texture)));
		BeginBatch();
	}

	size_t i = 0;
	for (; i < numOfAssignedSamplerSlots; i++)
	{
		if (texture == samplerSlots[i])
		{
			quadVertices[i * 4].in_tex = i;
			quadVertices[i * 4 + 1].in_tex = i;
			quadVertices[i * 4 + 2].in_tex = i;
			quadVertices[i * 4 + 3].in_tex = i;
			break;
		}
	}

	if (i == numOfAssignedSamplerSlots)
	{
		if (i >= 16)
		{
			EndBatch();
			BeginBatch();
			i = 0;
		}

		Graphics::BindTexture(texture, HN("btex_textures"), i);
		samplerSlots[i] = texture;

		quadVertices[i * 4].in_tex = i;
		quadVertices[i * 4 + 1].in_tex = i;
		quadVertices[i * 4 + 2].in_tex = i;
		quadVertices[i * 4 + 3].in_tex = i;

		numOfAssignedSamplerSlots++;
	}


	glm::mat4 modelmat = glm::translate(glm::mat4(1.0f), position);
	modelmat *= glm::mat4_cast(rotation);
	modelmat = glm::scale(modelmat, glm::vec3(scale, 1));

	quadVertices[spritesToPush * 4].ep_position = glm::vec3(modelmat * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f));
	quadVertices[spritesToPush * 4 + 1].ep_position = glm::vec3(modelmat * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f));
	quadVertices[spritesToPush * 4 + 2].ep_position = glm::vec3(modelmat * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));
	quadVertices[spritesToPush * 4 + 3].ep_position = glm::vec3(modelmat * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f));

	quadVertices[spritesToPush * 4].in_uv = glm::vec2(uv_l, uv_b);
	quadVertices[spritesToPush * 4 + 1].in_uv = glm::vec2(uv_r, uv_b);
	quadVertices[spritesToPush * 4 + 2].in_uv = glm::vec2(uv_r, uv_t);
	quadVertices[spritesToPush * 4 + 3].in_uv = glm::vec2(uv_l, uv_t);

	spritesToPush++;
	if (spritesToPush >= spriteComponents.size())
	{
		EndBatch();
		BeginBatch();
	}
}

void Renderer2D::EndBatch()
{
	if (!inBatch)
	{
		EP_WARN("Renderer2D::EndBatch(): Not currently assembling a sprite batch!");
		return;
	}

	Graphics::SetVertexData(quadVAH, quadVertices.data(), 0, spritesToPush * 4);
	Graphics::DrawTriangles(quadVAH, spritesToPush * 6, 0);
	Graphics::DisableShaderOption(HN("BATCHED_TEXTURES"));
	inBatch = false;
}

void Renderer2D::DrawSpriteComponents()
{
	BeginBatch();

	for (size_t i = 0; i < spriteComponentCount; i++)
	{
		DrawSprite(spriteComponents[i].tex,
			spriteComponents[i].uv_bounds[0],
			spriteComponents[i].uv_bounds[1],
			spriteComponents[i].uv_bounds[2],
			spriteComponents[i].uv_bounds[3],
			SceneManager::GetEntityPosition(spriteComponents[i].id),
			SceneManager::GetEntityRotation(spriteComponents[i].id),
			{
				SceneManager::GetEntityScale(spriteComponents[i].id).x,
				SceneManager::GetEntityScale(spriteComponents[i].id).y
			});
	}

	EndBatch();
}


static TextureFilter strToTexFilter(const std::string& str)
{
	if (str == "Nearest")
		return TextureFilter::Nearest;
	else if (str == "Linear")
		return TextureFilter::Linear;
	//else if (str == "Anisotropic")
		//return TextureFilter::Anisotropic;
	else
	{
		EP_WARN("Renderer2D::DeserializeSpriteComponent(): \"{}\" is not a TextureFilter!", str);
		return TextureFilter::Nearest;
	}
}

static MipmapMode strToMipMode(const std::string& str)
{
	if (str == "None")
		return MipmapMode::None;
	else if (str == "Nearest")
		return MipmapMode::Nearest;
	else if (str == "Linear")
		return MipmapMode::None;
	else
	{
		EP_WARN("Renderer2D::DeserializeSpriteComponent(): \"{}\" is not a MipmapMode!", str);
		return MipmapMode::None;
	}
}

static inline const char* texFilterToStr(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::Linear:
		return "Linear";
		break;
	//case TextureFilter::Anisotropic:
	//	return "Anisotropic";
	//	break;
	case TextureFilter::Nearest:
		return "Nearest";
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}

static inline const char* mipModeToStr(MipmapMode mode)
{
	switch (mode)
	{
	case MipmapMode::Linear:
		return "Linear";
		break;
	case MipmapMode::Nearest:
		return "Nearest";
		break;
	case MipmapMode::None:
		return "None";
		break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}


void Renderer2D::DeleteSpriteComponent(EntityID entity)
{
	Graphics::DeleteTexture(spriteComponents[spriteComponentIndices[entity]].tex);

	spriteComponents[spriteComponentIndices[entity]] = spriteComponents[spriteComponentCount - 1];
	spriteComponentIndices[spriteComponents[spriteComponentCount - 1].id] = spriteComponentIndices[entity];

	spriteComponentIndices.erase(entity);
	spriteComponentCount--;
}

std::vector<EntityID> Renderer2D::GetEntitiesWithSpriteComponents()
{
	std::vector<EntityID> returnVal;
	for (size_t i = 0; i < spriteComponentCount; i++)
	{
		returnVal.push_back(spriteComponents[i].id);
	}
	return returnVal;
}


bool Renderer2D::SerializeSpriteComponent(EntityID entity, YAML::Node& yamlOut)
{
	if (spriteComponentIndices.count(entity))
	{
		SpriteComponent& component = spriteComponents[spriteComponentIndices[entity]];
		
		yamlOut["Path"] = HN_ToStr(Graphics::GetTextureHashedPath(component.tex));
		yamlOut["UVs"]["l"] = component.uv_bounds[0];
		yamlOut["UVs"]["r"] = component.uv_bounds[1];
		yamlOut["UVs"]["b"] = component.uv_bounds[2];
		yamlOut["UVs"]["t"] = component.uv_bounds[3];
		yamlOut["MinFilter"] = texFilterToStr(minFilters[spriteComponentIndices[entity]]);
		yamlOut["MagFilter"] = texFilterToStr(magFilters[spriteComponentIndices[entity]]);
		yamlOut["MipMode"] = mipModeToStr(mipModes[spriteComponentIndices[entity]]);
		return true;
	}
	else
	{
		return false;
	}
}

bool Renderer2D::DeserializeSpriteComponent(EntityID entity, const YAML::Node& yamlIn)
{
	if (spriteComponentCount < spriteComponents.size())
	{
		if (yamlIn.Type() == YAML::NodeType::Map)
		{
			if (yamlIn["Path"] && yamlIn["UVs"] && yamlIn["MinFilter"] && yamlIn["MagFilter"] && yamlIn["MipMode"])
			{
				if (yamlIn["UVs"].Type() == YAML::NodeType::Map)
				{
					if (yamlIn["UVs"]["l"] &&
						yamlIn["UVs"]["r"] &&
						yamlIn["UVs"]["b"] &&
						yamlIn["UVs"]["t"])
					{
						try
						{
							spriteComponents[spriteComponentCount].uv_bounds[0] = yamlIn["UVs"]["l"].as<float>();
							spriteComponents[spriteComponentCount].uv_bounds[1] = yamlIn["UVs"]["r"].as<float>();
							spriteComponents[spriteComponentCount].uv_bounds[2] = yamlIn["UVs"]["b"].as<float>();
							spriteComponents[spriteComponentCount].uv_bounds[3] = yamlIn["UVs"]["t"].as<float>();
						}
						catch (YAML::BadConversion)
						{
							EP_ERROR("Renderer2D::DeserializeSpriteComponent(): Error reading UV subvalues.  "
								"Sprite component will not be loaded.  Entity: {}", entity);
							return false;
						}

						minFilters[spriteComponentCount] = strToTexFilter(yamlIn["MinFilter"].as<std::string>());
						magFilters[spriteComponentCount] = strToTexFilter(yamlIn["MagFilter"].as<std::string>());
						mipModes[spriteComponentCount] = strToMipMode(yamlIn["MipMode"].as<std::string>());

						spriteComponents[spriteComponentCount].tex =
							Graphics::LoadTexture(
								yamlIn["Path"].as<std::string>(),
								minFilters[spriteComponentCount],
								magFilters[spriteComponentCount],
								mipModes[spriteComponentCount]);

						if (spriteComponents[spriteComponentCount].tex == 0)
						{

							EP_ERROR("Renderer2D::DeserializeSpriteComponent(): Texture \"{}\" could not be loaded.  "
								"Sprite component will not be attached.  Entity: {}", entity);
							return false;
						}

						spriteComponents[spriteComponentCount].id = entity;
						spriteComponentIndices[entity] = spriteComponentCount;
						spriteComponentCount++;
						return true;
					}
					else
					{
						EP_ERROR("Renderer2D::DeserializeSpriteComponent(): Sprite component data not contain required subkeys!  ,"
							"Sprite component will not be loaded.  Entity: {}", entity);
						return false;
					}
				}
				else
				{
					EP_ERROR("Renderer2D::DeserializeSpriteComponent(): \"UVs\" subnode is not a map node!  ,"
						"Sprite component will not be loaded.  Entity: {}", entity);
					return false;
				}
			}
			else
			{
				EP_ERROR("Renderer2D::DeserializeSpriteComponent(): Sprite component data does not contain required subkeys!  ,"
					"Sprite component will not be loaded.  Entity: {}", entity);
				return false;
			}
		}
		else
		{
			EP_ERROR("Renderer2D::DeserializeSpriteComponent(): Sprite component data node is not a map node!  ,"
				"Sprite component will not be loaded.  Entity: {}", entity);
			return false;
		}
	}
	else
	{
		EP_ERROR("Renderer2D::DeserializeSpriteComponent(): Exhausted sprite component buffers!");
		return false;
	}
}

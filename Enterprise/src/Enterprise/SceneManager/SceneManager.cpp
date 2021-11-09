#include "EP_PCH.h"
#include "SceneManager.h"
#include "../File/File.h"
#include "../Events/Events.h"

namespace Enterprise
{

static std::vector<SceneManager::DelComponentFn> dcfs;
static std::map<HashName, SceneManager::QueryComponentFn> qcfs;
static std::vector<std::pair<HashName, SceneManager::TextSerializeFn>> tsfs;
static std::map<HashName, SceneManager::TextDeserializeFn> tdfs;

void SceneManager::RegisterComponentType(HashName name,
	DelComponentFn dcf, QueryComponentFn qcf,
	TextSerializeFn tsf, TextDeserializeFn tdf)
{
	EP_ASSERT(qcfs.count(name) == 0);
	EP_ASSERT(dcf);
	EP_ASSERT(qcf);
	EP_ASSERT(tsf);
	EP_ASSERT(tdf);

	dcfs.push_back(dcf);
	qcfs[name] = qcf;
	tsfs.emplace_back(std::pair(name, tsf));
	tdfs[name] = tdf;
}


struct Entity
{
	SceneManager::EntityID id = 0;
	HashName name = HN("");
	//SceneManager::EntityID parent;
	//std::set<SceneManager::EntityID> children;
	//std::set<HashName> tags;
	Math::Vec3 position;
	Math::Vec3 rotation;
};
static std::vector<Entity> entityPool(Constants::MaxEntities);

static std::map<SceneManager::EntityID, size_t> entityIndices;
static std::vector<size_t> availableEntityPoolIndices(Constants::MaxEntities);
static std::vector<SceneManager::EntityID> availableSpawnedIDs(Constants::MaxSpawnedEntities);

static SceneManager::EntityID genSpawnedEntityID()
	// Helper function: returns a unique EntityID in range [1, Constants::MaxSpawnedIDs].
{
	SceneManager::EntityID returnVal = availableSpawnedIDs.back();
	availableSpawnedIDs.pop_back();
	return returnVal;
}

SceneManager::EntityID SceneManager::CreateEntity(HashName name, Math::Vec3 position, Math::Vec3 rotation)
{
	EntityID id = genSpawnedEntityID();
	entityPool[availableEntityPoolIndices.back()] = { id, name, position, rotation };
	entityIndices[id] = availableEntityPoolIndices.back();
	availableEntityPoolIndices.pop_back();
	return id;
}

void SceneManager::DeleteEntity(EntityID entity)
{
	if (entityIndices.count(entity) != 0)
	{
		// Delete any attached components
		for (const DelComponentFn& dcf : dcfs)
		{
			dcf(entity);
		}

		// Mark entity as deleted
		if (entity <= Constants::MaxSpawnedEntities)
		{
			availableSpawnedIDs.push_back(entity);
		}
		availableEntityPoolIndices.push_back(entityIndices[entity]);
		entityIndices.erase(entity);

		Events::Dispatch(HN("EntityDeleted"), entity);
	}
	else
	{
		EP_WARN("SceneManager::DeleteEntity(): EntityID {} does not exist!", entity);
	}
}

void SceneManager::PurgeSpawnedEntities()
{
	while (entityIndices.size() > 0)
	{
		EntityID id = entityIndices.begin()->first;
		if (id > Constants::MaxSpawnedEntities)
			return;

		// Delete any attached components
		for (const DelComponentFn& dcf : dcfs)
		{
			dcf(id);
		}

		// Mark entity as deleted
		availableSpawnedIDs.push_back(id);
		availableEntityPoolIndices.push_back(entityIndices.begin()->second);
		entityIndices.erase(entityIndices.begin());

		Events::Dispatch(HN("EntityDeleted"), id);
	}
}


bool SceneManager::IsEntityValid(EntityID entity)
{
	return entityIndices.count(entity);
}


Math::Vec3 SceneManager::GetEntityPosition(EntityID entity)
{
	if (entityIndices.count(entity) != 0)
	{
		return entityPool.at(entityIndices[entity]).position;
	}
	else
	{
		EP_ERROR("SceneManager::GetEntityPosition(): EntityID {} does not exist!", entity);
		return 0;
	}
}

Math::Vec3 SceneManager::GetEntityRotation(EntityID entity)
{
	if (entityIndices.count(entity) != 0)
	{
		return entityPool.at(entityIndices[entity]).rotation;
	}
	else
	{
		EP_ERROR("SceneManager::GetEntityRotation(): EntityID {} does not exist!", entity);
		return 0;
	}
}


std::vector<SceneManager::EntityID> SceneManager::GetEntitiesWithComponent(HashName componentType)
{
	if (qcfs.count(componentType) != 0)
	{
		return qcfs[componentType]();
	}
	else
	{
		EP_ERROR("SceneManager::GetEntitiesWithComponent(): \"{}\" is not a valid component type.  "
			"Did you register the required system using SceneManager::RegisterComponentType()?",
			HN_ToStr(componentType));
		return {};
	}
}


void SceneManager::SaveEntitiesToTextFile(std::string path, const std::vector<EntityID>& entities)
{
	YAML::Emitter outYaml;
	outYaml << YAML::BeginMap; // File-wide

	// Entities
	outYaml << YAML::Key << "Entities" << YAML::Value << YAML::BeginMap;
	if (entities.size() == 0)
	{
		for (const auto& [id, index] : entityIndices)
		{
			outYaml << YAML::Key << id << YAML::Value << YAML::BeginMap;
			outYaml << YAML::Key << "Name" << YAML::Value << HN_ToStr(entityPool.at(index).name);
			outYaml << YAML::Key << "Position" << YAML::Value << YAML::BeginMap;
			outYaml << YAML::Key << "x" << YAML::Value << entityPool.at(index).position.x;
			outYaml << YAML::Key << "y" << YAML::Value << entityPool.at(index).position.y;
			outYaml << YAML::Key << "z" << YAML::Value << entityPool.at(index).position.z;
			outYaml << YAML::EndMap;
			outYaml << YAML::Key << "Rotation" << YAML::Value << YAML::BeginMap;
			outYaml << YAML::Key << "roll" << YAML::Value << entityPool.at(index).rotation.x;
			outYaml << YAML::Key << "pitch" << YAML::Value << entityPool.at(index).rotation.y;
			outYaml << YAML::Key << "yaw" << YAML::Value << entityPool.at(index).rotation.z;
			outYaml << YAML::EndMap;
			outYaml << YAML::EndMap;
		}
	}
	else
	{
		for (EntityID id : entities)
		{
			if (entityIndices.count(id) != 0)
			{
				outYaml << YAML::Key << id << YAML::Value << YAML::BeginMap;
				outYaml << YAML::Key << "Name" << YAML::Value << HN_ToStr(entityPool.at(entityIndices[id]).name);
				outYaml << YAML::Key << "Position" << YAML::Value << YAML::BeginMap;
				outYaml << YAML::Key << "x" << YAML::Value << entityPool.at(entityIndices[id]).position.x;
				outYaml << YAML::Key << "y" << YAML::Value << entityPool.at(entityIndices[id]).position.y;
				outYaml << YAML::Key << "z" << YAML::Value << entityPool.at(entityIndices[id]).position.z;
				outYaml << YAML::EndMap;
				outYaml << YAML::Key << "Rotation" << YAML::Value << YAML::BeginMap;
				outYaml << YAML::Key << "roll" << YAML::Value << entityPool.at(entityIndices[id]).rotation.x;
				outYaml << YAML::Key << "pitch" << YAML::Value << entityPool.at(entityIndices[id]).rotation.y;
				outYaml << YAML::Key << "yaw" << YAML::Value << entityPool.at(entityIndices[id]).rotation.z;
				outYaml << YAML::EndMap;
				outYaml << YAML::EndMap;
			}
			else
			{
				EP_WARN("SceneManager::SaveEntitiesToTextFile(): EntityID {} does not exist!", id);
			}
		}
	}
	outYaml << YAML::EndMap; // Entities section

	// Components
	outYaml << YAML::Key << "Components" << YAML::Value << YAML::BeginMap;
	for (const auto& [componentTypeName, tsf] : tsfs)
	{
		outYaml << YAML::Key << HN_ToStr(componentTypeName) << YAML::Value << YAML::BeginMap;

		if (entities.size() == 0)
		{
			for (const auto& [id, index] : entityIndices)
			{
				YAML::Node callbackNode;
				try
				{
					if (tsf(id, callbackNode))
					{
						outYaml << YAML::Key << id << YAML::Value << callbackNode;
					}
				}
				catch (const YAML::Exception& except)
				{
					EP_ERROR("SceneManager::SaveEntitiesToTextFile(): YAML exception during {} serialization!  "
						"Message: {}", HN_ToStr(componentTypeName), except.msg);
				}
			}
		}
		else
		{
			for (EntityID id : entities)
			{
				YAML::Node callbackNode;
				try
				{
					if (tsf(id, callbackNode))
					{
						outYaml << YAML::Key << id << YAML::Value << callbackNode;
					}
				}
				catch (const YAML::Exception& except)
				{
					EP_ERROR("SceneManager::SaveEntitiesToTextFile(): YAML exception during {} serialization!  "
						"Message: {}", HN_ToStr(componentTypeName), except.msg);
				}
			}
		}

		outYaml << YAML::EndMap; // Component type section
	}
	outYaml << YAML::EndMap; // Components section

	outYaml << YAML::EndMap; // File-wide
	File::SaveTextFile(path, outYaml.c_str());
}

bool SceneManager::LoadEntitiesFromYAML(const std::string& yamlSrc)
{
	try
	{
		YAML::Node yamlIn = YAML::Load(yamlSrc);

		if (yamlIn.Type() != YAML::NodeType::Map)
		{
			EP_ERROR("SceneManager::LoadEntitiesFromYAML(): Root node is not a map node!");
			return false;
		}
		if (!yamlIn["Entities"])
		{
			EP_ERROR("SceneManager::LoadEntitiesFromYAML(): \"Entities\" key not present in root node!");
			return false;
		}
		if (yamlIn["Entities"].Type() != YAML::NodeType::Map)
		{
			EP_ERROR("SceneManager::LoadEntitiesFromYAML(): \"Entities\" key is not associated with a map node!");
			return false;
		}

		std::map<EntityID, EntityID> oldIDtoNewID;

		// Entities
		for (auto entityDataIt = yamlIn["Entities"].begin();
			entityDataIt != yamlIn["Entities"].end();
			entityDataIt++)
		{
			if (entityDataIt->second.Type() != YAML::NodeType::Map)
			{
				EP_WARN("SceneManager::LoadEntitiesFromYAML(): Entity subnode is not a mapping.  "
					"ID: {}", entityDataIt->first.as<std::string>());
			}
			else if (!entityDataIt->second["Name"] ||
				!entityDataIt->second["Position"] ||
				!entityDataIt->second["Rotation"])
			{
				EP_WARN("SceneManager::LoadEntitiesFromYAML() : Entity subnode is missing data.  "
					"ID : {}", entityDataIt->first.as<std::string>());
			}
			else
			{
				if (entityDataIt->second["Position"].Type() != YAML::NodeType::Map ||
					entityDataIt->second["Rotation"].Type() != YAML::NodeType::Map)
				{
					EP_WARN("SceneManager::LoadEntitiesFromYAML() : \"Position\" or \"Rotation\" subvalues "
						"for entity \"{}\" are not mappings.", entityDataIt->first.as<std::string>());
				}
				else if (!entityDataIt->second["Position"]["x"] || 
					!entityDataIt->second["Position"]["y"] || 
					!entityDataIt->second["Position"]["z"] ||
					!entityDataIt->second["Rotation"]["roll"] || 
					!entityDataIt->second["Rotation"]["pitch"] || 
					!entityDataIt->second["Rotation"]["yaw"])
				{
					EP_WARN("SceneManager::LoadEntitiesFromYAML() : \"Position\" or \"Rotation\" subvalues "
						"for entity \"{}\" are missing data.", entityDataIt->first.as<std::string>());
				}
				else
				{
					EntityID id;

					try
					{
						// Non-string deserialization done first because they throw exceptions in case of failure
						id = entityDataIt->first.as<EntityID>();
						Math::Vec3 outPos =
						{
							entityDataIt->second["Position"]["x"].as<float>(),
							entityDataIt->second["Position"]["y"].as<float>(),
							entityDataIt->second["Position"]["z"].as<float>()
						};
						Math::Vec3 outRot =
						{
							entityDataIt->second["Rotation"]["roll"].as<float>(),
							entityDataIt->second["Rotation"]["pitch"].as<float>(),
							entityDataIt->second["Rotation"]["yaw"].as<float>()
						};


						if (id <= Constants::MaxSpawnedEntities)
							// Spawnable range ID: Always create new
						{
							oldIDtoNewID[id] = genSpawnedEntityID();
							entityIndices[oldIDtoNewID[id]] = availableEntityPoolIndices.back();
							availableEntityPoolIndices.pop_back();
						}
						else
							// Scene file range ID: Update existing entity, if it exists
						{
							oldIDtoNewID[id] = id;

							if (entityIndices.count(id))
								// Entity in scene already: delete attached components
							{
								for (const DelComponentFn& dcf : dcfs)
								{
									dcf(id);
								}
							}
							else
								// Entity not in scene yet: create one
							{
								entityIndices[id] = availableEntityPoolIndices.back();
								availableEntityPoolIndices.pop_back();
							}
						}

						// Set entity data
						entityPool[entityIndices[oldIDtoNewID[id]]].name = 
							HN(entityDataIt->second["Name"].as<std::string>());
						entityPool[entityIndices[oldIDtoNewID[id]]].position = outPos;
						entityPool[entityIndices[oldIDtoNewID[id]]].rotation = outRot;
					}
					catch (const YAML::TypedBadConversion<EntityID>&)
					{
						EP_WARN("SceneManager::LoadEntitiesFromYAML(): Invalid EntityID in entities list: {}",
							entityDataIt->first.as<std::string>());
					}
					catch (const YAML::TypedBadConversion<float>& exc)
					{
						EP_WARN("SceneManager::LoadEntitiesFromYAML(): Cannot deserialize EntityID {}: "
							"Invalid position or rotation value.", id);
					}
				}
			}
		}

		// Components
		if (yamlIn["Components"])
		{
			if (yamlIn["Components"].Type() == YAML::NodeType::Map)
			{
				for (auto componentTypeIt = yamlIn["Components"].begin();
					componentTypeIt != yamlIn["Components"].end();
					componentTypeIt++)
				{
					if (componentTypeIt->second.Type() == YAML::NodeType::Map)
					{
						HashName componentTypeName = HN(componentTypeIt->first.as<std::string>());
						for (auto componentDataIt = componentTypeIt->second.begin();
							componentDataIt != componentTypeIt->second.end();
							componentDataIt++)
						{
							if (tdfs.count(componentTypeName))
							{
								try
								{
									EntityID id = componentDataIt->first.as<EntityID>();
									if (oldIDtoNewID.count(id) != 0)
									{
										try
										{
											if (!tdfs[componentTypeName](oldIDtoNewID[id], componentDataIt->second))
											{
												EP_WARN("SceneManager::LoadEntitiesFromYAML(): "
													"Could not deserialize {} component data for entity {}.",
													HN_ToStr(componentTypeName), id);
											}
										}
										catch (const YAML::Exception& except)
										{
											EP_ERROR("SceneManager::LoadEntitiesFromYAML(): "
												"YAML exception during {} component deserialization!  Message: {}", 
												HN_ToStr(componentTypeName), except.msg);
										}
									}
									else
									{
										EP_WARN("SceneManager::LoadEntitiesFromYAML(): {} component data found for "
											"EntityID that isn't present in \"Entities\" section!  Component will "
											"not be loaded.  EntityID: {}", HN_ToStr(componentTypeName), id);
									}
								}
								catch (const YAML::TypedBadConversion<EntityID>&)
								{
									EP_WARN("SceneManager::LoadEntitiesFromYAML(): "
										"Invalid EntityID in \"{}\" component section: {}",
										HN_ToStr(componentTypeName), componentDataIt->first.as<std::string>());
								}
							}
							else
							{
								EP_WARN("SceneManager::LoadEntitiesFromYAML(): \"{}\" is not a valid component type.  "
									"Did you register the required system using SceneManager::RegisterComponentType()?",
									HN_ToStr(componentTypeName));
							}
						}
					}
					else
					{
						EP_WARN("SceneManager::LoadEntitiesFromYAML(): \"Components\" subkey \"{}\" is not associated"
							" with a mapping!  \"{}\" components will not be deserialized.",
							componentTypeIt->first.as<std::string>());
					}
				}
			}
			else
			{
				EP_WARN("SceneManager::LoadEntitiesFromYAML(): \"Components\" key is not associated with a mapping!"
					"  No component data can be read.");
			}
		}
		else
		{
			EP_WARN("SceneManager::LoadEntitiesFromYAML(): File does not contain \"Components\" section.");
		}
	}
	catch (const YAML::Exception& e)
	{
		EP_ERROR("SceneManager::LoadEntitiesFromYAML(): YAML exception thrown!  Message: {}", e.msg);
		return false;
	}

	return true;
}

bool SceneManager::LoadEntitiesFromTextFile(const std::string& path)
{
	std::string src;
	if (File::LoadTextFile(path, &src) == File::ErrorCode::Success)
	{
		bool result = LoadEntitiesFromYAML(src);
		if (!result)
		{
			EP_ERROR("SceneManager::LoadEntitiesFromTextFile(): Failure loading entities from file \"{}\".", path);
		}
		return result;
	}
	else
	{
		return false;
	}
}


static std::vector<SceneManager::CoreCallFn> fixedUpdateCallbacks;
static std::vector<SceneManager::CoreCallFn> updateCallbacks;
static std::vector<SceneManager::CoreCallFn> predrawCallbacks;
static std::vector<SceneManager::CoreCallFn> drawCallbacks;

void SceneManager::RegisterFixedUpdateFn(CoreCallFn func)
{
	EP_ASSERT(func);
	fixedUpdateCallbacks.push_back(func);
}

void SceneManager::RegisterUpdateFn(CoreCallFn func)
{
	EP_ASSERT(func);
	updateCallbacks.push_back(func);
}

void SceneManager::RegisterPreDrawFn(CoreCallFn func)
{
	EP_ASSERT(func);
	predrawCallbacks.push_back(func);
}

void SceneManager::RegisterSceneDrawFn(CoreCallFn func)
{
	EP_ASSERT(func);
	drawCallbacks.push_back(func);
}


void SceneManager::DrawScene()
{
	for (CoreCallFn c : drawCallbacks)
	{
		c();
	}
}


void SceneManager::Init()
{
	EP_ASSERT(Constants::MaxSpawnedEntities < Constants::MaxEntities);

	for (size_t i = 0; i < Constants::MaxEntities; i++)
	{
		availableEntityPoolIndices[i] = Constants::MaxEntities - 1 - i;
	}
	for (size_t i = 0; i < Constants::MaxSpawnedEntities; i++)
	{
		availableSpawnedIDs[i] = Constants::MaxSpawnedEntities - i;
	}
}

void SceneManager::FixedUpdate()
{
	for (CoreCallFn c : fixedUpdateCallbacks)
	{
		c();
	}
}

void SceneManager::Update()
{
	for (CoreCallFn c : updateCallbacks)
	{
		c();
	}
}

void SceneManager::PreDraw()
{
	for (CoreCallFn c : predrawCallbacks)
	{
		c();
	}
}

}

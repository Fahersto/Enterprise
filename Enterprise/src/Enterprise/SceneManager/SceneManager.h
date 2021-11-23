#pragma once
#include "EP_PCH.h"
#include "Core.h"

/// A unique identifier for a scene entity.
typedef uint64_t EntityID;

namespace Enterprise
{

namespace Constants
{
/// The maximum number of entities allowed in the scene simultaneously.
extern const size_t MaxEntities;
/// The maximum number of entities that can be spawned at runtime.
extern const size_t MaxSpawnedEntities;
}

/// Enterprise's global entity system.
class SceneManager
{
public:

    // ECS stuff

    /// A pointer to a component deletion callback.  Deletes all components of a type from an entity.
    typedef void (*DelComponentFn)(EntityID entity);
    /// A pointer to a component query callback.  Returns all EntityIDs with a component type attached.
    typedef std::vector<EntityID>(*QueryComponentFn)();
    /// A pointer to a text serialization callback for a component type.
    typedef bool (*TextSerializeFn)(EntityID entity, YAML::Node& yamlOut);
    /// A pointer to a text deserialization callback for a component type.
    typedef bool (*TextDeserializeFn)(EntityID entity, const YAML::Node& yamlIn);

    /// Register a new component type with SceneManager.
    /// @param name The HashName of the new component type.
    /// @param dcf Pointer to the delete function for this component type.
    /// @param qcf Pointer to the query function for this component type.
    /// @param tsf Pointer to the serialization function for this component type.
    /// @param tdf Pointer to the deserialization function for this component type.
    static void RegisterComponentType(HashName name,
        DelComponentFn dcf, QueryComponentFn qcf,
        TextSerializeFn tsf, TextDeserializeFn tdf);

    /// Spawn a new, empty entity.
    /// @param name The HashName of the entity.
    /// @param position The starting position of the entity.
    /// @param rotation The starting orientation of the entity.
    /// @param scale The starting scale of the entity.
    /// @return The ID of the created entity.
    static EntityID CreateEntity(HashName name,
        glm::vec3 position = glm::vec3(),
        glm::quat rotation = glm::quat(),
        glm::vec3 scale = glm::vec3());
    /// Remove an entity from the scene.
    /// @param entity The ID of the entity to delete.
    static void DeleteEntity(EntityID entity);
    /// Remove all spawned entities from the scene.
    /// @note This function does not affect entities that were created by loading a scene file.
    static void PurgeSpawnedEntities();

    /// Check whether an entity currently exists in the scene.
    /// @param entity The EntityID to check.
    /// @return @c true if entity exists.
    static bool IsEntityValid(EntityID entity);

    /// Get the position of an entity.
    /// @param entity The ID of the entity to query.
    /// @return The position of the entity, expressed as a vec3.
    static glm::vec3 GetEntityPosition(EntityID entity);
    /// Get the rotation of an entity.
    /// @param entity The ID of the entity to query.
    /// @return The entity's orientation, expressed as a quaternion.
    static glm::quat GetEntityRotation(EntityID entity);
    /// Get the scale values of an entity.
    /// @param entity The ID of the entity to query.
    /// @return The entity's scale values, expressed as a vec3.
    static glm::vec3 GetEntityScale(EntityID entity);

    /// Get the IDs of all entities with a specific component type attached.
    /// @param componentType The HashName of the component type.
    /// @return A vector of all EntityIDs associated with at least one component of the given type.
    static std::vector<EntityID> GetEntitiesWithComponent(HashName componentType);

    /// Serialize entities to a text file.
    /// @param path The virtual path to the destination file.
    /// @param entities The IDs of the entities to serialize.  If empty, all entities will be serialized.
    static void SaveEntitiesToTextFile(std::string path, const std::vector<EntityID>& entities = {});
    /// Populate the scene with entities serialized in a string.
    /// @param yamlSrc A string containing entities serialized in YAML format.
    /// @return @c true if deserialization was successful.
    /// @remarks Entities originally from a scene file are replaced if already present in the scene.
    /// All other entities are simply added to the scene, which can cause entity duplication.
    static bool LoadEntitiesFromYAML(const std::string& yamlSrc);
    /// Populate the scene with entities serialized in a text file.
    /// @param path The virtual path to the file to load.
    /// @return @c true if deserialization was successful.
    /// @remarks Entities originally from a scene file are replaced if already present in the scene.
    /// All other entities are simply added to the scene, which can cause entity duplication.
    static bool LoadEntitiesFromTextFile(const std::string& path);

    // System stuff

    /// A pointer to an Update(), FixedUpdate(), PreDraw(), or SceneDraw() function.
    typedef void(*CoreCallFn)();
    /// Register a FixedUpdate() callback.
    /// @param func Pointer to the @c FixedUpdate() callback.
    static void RegisterFixedUpdateFn(CoreCallFn func);
    /// Register an Update() callback.
    /// @param func Pointer to the @c Update() callback.
    static void RegisterUpdateFn(CoreCallFn func);
    /// Register a PreDraw() callback.
    /// @param func Pointer to the @c PreDraw() callback.
    static void RegisterPreDrawFn(CoreCallFn func);
    /// Register a SceneDraw() callback.
    /// @param func Pointer to the @c SceneDraw() callback.
    /// @remarks @c SceneDraw() callbacks are invoked every time SceneManager::DrawScene() is invoked.
    /// This may occur multiple times per frame.
    static void RegisterSceneDrawFn(CoreCallFn func);

    /// Render the scene with the current camera and render settings.
    static void DrawScene();

private:
	friend class Application;

    static void Init();
	static void FixedUpdate();
	static void Update();
    static void PreDraw();
};

}

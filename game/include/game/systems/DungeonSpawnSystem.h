#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"
#include "engine/scene/PrefabManager.h"
#include "engine/scene/BuildRoomSystem.h"
#include "engine/resources/MeshManager.h"
#include "engine/resources/AssetManager.h"

#include <glm/glm.hpp>

namespace game
{
    class DungeonSpawnSystem
    {
    public:
        DungeonSpawnSystem(engine::Registry& registry,
                           engine::MeshManager& meshManager,
                           engine::AssetManager& assetManager);

        void SpawnDungeon(int roomCount, int seed, float mazeFactor);

        engine::Entity SpawnPrefab(engine::PrefabType type,
                                    const glm::vec3& position,
                                    float rotY);

    private:
        static bool IsWallPrefab(engine::PrefabType type);
        void SpawnColliderEntity(const glm::vec3& position, float width, float height, float depth);
        void SpawnWallColliders(engine::PrefabType type, const glm::vec3& position, float rotY);
        void SpawnDoorwayPillarColliders(const glm::vec3& position, float rotY);
        void SpawnEnemies(const engine::MapGrid& map, const engine::BuildRoomConfig& config, int seed);

        engine::Registry& m_Registry;
        engine::MeshManager& m_MeshManager;
        engine::AssetManager& m_AssetManager;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H

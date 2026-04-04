// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"
#include "engine/scene/PrefabManager.h"
#include "engine/scene/BuildRoomSystem.h"
#include "engine/resources/MeshManager.h"
#include "engine/resources/AssetManager.h"
#include "engine/rendering/AnimationData.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace game
{
    class DungeonSpawnSystem
    {
    public:
        DungeonSpawnSystem(engine::Registry& registry,
                           engine::MeshManager& meshManager,
                           engine::AssetManager& assetManager);

        void SpawnDungeon(int roomCount, int seed, float mazeFactor);
        void SpawnEnemies(engine::MapGrid& map, const engine::BuildRoomConfig& config, int seed);
        void SpawnButtons(engine::MapGrid& map, const engine::BuildRoomConfig& config, int seed);

        engine::Entity SpawnPrefab(engine::PrefabType type,
                                    const glm::vec3& position,
                                    float rotY);

    private:
        static bool IsWallPrefab(engine::PrefabType type);
        void SpawnColliderEntity(const glm::vec3& position, float width, float height, float depth);
        void SpawnWallColliders(engine::PrefabType type, const glm::vec3& position, float rotY);
        void SpawnDoorwayPillarColliders(const glm::vec3& position, float rotY);

        engine::Registry& m_Registry;
        engine::MeshManager& m_MeshManager;
        engine::AssetManager& m_AssetManager;

    public:
        std::shared_ptr<std::vector<engine::AnimationClip>> SharedClips;
        std::shared_ptr<engine::Skeleton> EnemySkeleton;
        std::vector<engine::Entity> SpawnedDoors;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H

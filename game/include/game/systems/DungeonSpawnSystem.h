// Created by Adam Van Woerden

// DungeonSpawnSystem -- Procedurally generates a dungeon floor and
// populates it with prefab geometry, wall/door colliders, enemies, and
// puzzle buttons. Coordinates with the engine's FloorGenerator and
// BuildRoomSystem to turn a grid layout into a playable 3D level.

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

        // Reprints the generated map grid to stdout for debugging.
        void DebugPrintMap() const;

        // World-space dimensions of the generated map (in game units).
        [[nodiscard]] float GetMapWorldWidth() const { return m_MapWorldWidth; }
        [[nodiscard]] float GetMapWorldDepth() const { return m_MapWorldDepth; }

        // World-space position of the Start tile (player spawn). Returns
        // (0,0,0) and HasStartPos()==false if no Start cell was produced.
        [[nodiscard]] bool HasStartPos() const { return m_HasStartPos; }
        [[nodiscard]] const glm::vec3& GetStartWorldPos() const { return m_StartWorldPos; }

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

        engine::MapGrid m_LastMap;
        float m_MapWorldWidth = 0.0f;
        float m_MapWorldDepth = 0.0f;
        glm::vec3 m_StartWorldPos = glm::vec3(0.0f);
        bool m_HasStartPos = false;

    public:
        std::shared_ptr<std::vector<engine::AnimationClip>> SharedClips; // Shared animation clips for enemies
        std::shared_ptr<engine::Skeleton> EnemySkeleton;                 // Cached skeleton rig for enemy meshes
        std::vector<engine::Entity> SpawnedDoors; // Doors available for puzzle-button linking
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DUNGEONSPAWNSYSTEM_H

#include "game/systems/DungeonSpawnSystem.h"

#include "game/components/Transform.h"
#include "game/components/Render.h"
#include "game/components/Collider.h"
#include "game/components/Door.h"
#include "game/components/EnemyAI.h"
#include "game/components/Health.h"
#include "game/components/CombatState.h"

#include "engine/scene/BuildRoomSystem.h"
#include "engine/scene/FloorGenerator.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>
#include <random>

namespace game
{
    DungeonSpawnSystem::DungeonSpawnSystem(engine::Registry& registry,
                                           engine::MeshManager& meshManager,
                                           engine::AssetManager& assetManager)
        : m_Registry(registry),
          m_MeshManager(meshManager),
          m_AssetManager(assetManager)
    {
    }

    void DungeonSpawnSystem::SpawnDungeon(int roomCount, int seed, float mazeFactor)
    {
        engine::FloorConfig config;
        config.roomCount = roomCount;
        config.seed = seed;
        config.mazeFactor = mazeFactor;

        auto dungeon = engine::FloorGenerator::Generate(config);
        auto map = engine::BuildRoomSystem::FromFloor(dungeon);

        engine::BuildRoomSystem::DebugPrint(map);
        auto instances = engine::BuildRoomSystem::Build(map, config.buildConfig);

        for (const auto& inst : instances)
        {
            SpawnPrefab(inst.prefab, inst.position, inst.rotationY);
            if (inst.prefab == engine::PrefabType::Stairs && inst.position.y < -1)
            {
                SpawnPrefab(engine::PrefabType::WallCornerSmall,
                    inst.position + glm::vec3{2, 0, 4.5}, inst.rotationY - 90);
                SpawnPrefab(engine::PrefabType::Wall,
                    inst.position + glm::vec3{0, 0, 4.5}, inst.rotationY);
                SpawnPrefab(engine::PrefabType::WallCornerSmall,
                    inst.position + glm::vec3{-2, 0, 4.5}, inst.rotationY + 180);
            }
        }

        // Spawn enemies on random floor tiles
        SpawnEnemies(map, config.buildConfig, seed);
    }

    void DungeonSpawnSystem::SpawnEnemies(const engine::MapGrid& map,
                                           const engine::BuildRoomConfig& config,
                                           int seed)
    {
        std::mt19937 rng(seed + 999);

        // Collect floor tiles (not start/end/door/wall)
        std::vector<glm::vec3> floorPositions;
        for (int y = 0; y < map.height; ++y)
        {
            for (int x = 0; x < map.width; ++x)
            {
                engine::CellType cell = map.cells[y * map.width + x];
                if (cell == engine::CellType::Floor)
                {
                    float worldX = x * config.tileSize - config.floorOffset;
                    float worldZ = y * config.tileSize - config.floorOffset;
                    floorPositions.emplace_back(worldX, 0.0f, worldZ);
                }
            }
        }

        if (floorPositions.empty())
            return;

        // Shuffle and pick a subset for enemy placement
        std::shuffle(floorPositions.begin(), floorPositions.end(), rng);

        int enemyCount = std::min(static_cast<int>(floorPositions.size() / 8), 5);
        if (enemyCount < 1) enemyCount = 1;

        // Load enemy mesh
        auto enemyMeshResult = m_MeshManager.Load("asset/Rogue_Hooded.glb");
        auto enemyTexture = m_AssetManager.GetTextureManager().Load("asset/rogue_texture.png");

        if (!enemyMeshResult.MeshPtr || !enemyTexture)
        {
            std::cerr << "[DungeonSpawnSystem] Failed to load enemy model\n";
            return;
        }

        for (int i = 0; i < enemyCount; ++i)
        {
            const glm::vec3& pos = floorPositions[i];

            engine::Entity enemy = m_Registry.CreateEntity();

            Transform t(pos.x, pos.y, pos.z);
            m_Registry.AddComponent(enemy, t);
            m_Registry.AddComponent(enemy, Render(enemyMeshResult.MeshPtr, enemyTexture));

            Collider c(0.6f, 1.8f, 0.6f);
            c.IsStatic = false;
            m_Registry.AddComponent(enemy, c);

            m_Registry.AddComponent(enemy, EnemyAI(2.0f));
            m_Registry.AddComponent(enemy, Health(3, 3));
            m_Registry.AddComponent(enemy, CombatState());

            std::cout << "[DungeonSpawnSystem] Spawned enemy at ("
                      << pos.x << ", " << pos.z << ")\n";
        }
    }

    engine::Entity DungeonSpawnSystem::SpawnPrefab(
        engine::PrefabType type,
        const glm::vec3& position,
        const float rotY)
    {
        const auto* def = engine::PrefabManager::GetRandom(type);
        if (!def)
        {
            std::cerr << "[DungeonSpawnSystem] Missing prefab\n";
            return engine::Entity(0);
        }

        auto mesh = m_MeshManager.Load(def->meshPath);
        auto texture = m_AssetManager.GetTextureManager().Load(def->texturePath);

        if (!mesh.MeshPtr || !texture)
        {
            std::cerr << "[DungeonSpawnSystem] Failed to load: " << def->meshPath << "\n";
            return engine::Entity(0);
        }

        engine::Entity e = m_Registry.CreateEntity();

        Transform t(position.x, position.y, position.z);
        t.RotationY = glm::radians(rotY);

        m_Registry.AddComponent(e, t);
        m_Registry.AddComponent(e, Render(mesh.MeshPtr, texture));

        if (IsWallPrefab(type))
        {
            SpawnWallColliders(type, position, rotY);
        }

        if (type == engine::PrefabType::WallDoorway)
        {
            SpawnDoorwayPillarColliders(position, rotY);
        }

        if (type == engine::PrefabType::Door)
        {
            // The door entity is offset from the doorway center by doorOffset (0.82).
            // Place the trigger at the doorway opening, not at the door hinge.
            float doorOffset = 0.82f;
            float normRot = std::fmod(rotY, 360.0f);
            if (normRot < 0.0f) normRot += 360.0f;

            Collider doorTrigger(2.5f, 3.0f, 2.5f);
            doorTrigger.IsTrigger = true;
            doorTrigger.IsStatic = false;

            // Offset the trigger collider back toward the doorway center
            int rot = static_cast<int>(normRot + 0.5f) % 360;
            if (rot < 45 || rot >= 315)
                doorTrigger.OffsetX = doorOffset;
            else if (rot < 135)
                doorTrigger.OffsetZ = -doorOffset;
            else if (rot < 225)
                doorTrigger.OffsetX = -doorOffset;
            else
                doorTrigger.OffsetZ = doorOffset;

            m_Registry.AddComponent(e, doorTrigger);

            Door door;
            door.BaseRotationY = glm::radians(rotY);
            m_Registry.AddComponent(e, door);
        }

        return e;
    }

    bool DungeonSpawnSystem::IsWallPrefab(engine::PrefabType type)
    {
        return type == engine::PrefabType::Wall ||
               type == engine::PrefabType::WallCorner ||
               type == engine::PrefabType::WallCrossing ||
               type == engine::PrefabType::WallTsplit ||
               type == engine::PrefabType::WallCornerSmall;
    }

    void DungeonSpawnSystem::SpawnColliderEntity(const glm::vec3& position, float width, float height, float depth)
    {
        engine::Entity e = m_Registry.CreateEntity();
        m_Registry.AddComponent(e, Transform(position.x, position.y, position.z));

        Collider c(width, height, depth);
        c.IsStatic = true;
        m_Registry.AddComponent(e, c);
    }

    void DungeonSpawnSystem::SpawnWallColliders(engine::PrefabType type, const glm::vec3& position, float rotY)
    {
        constexpr float armLength = 4.0f;
        constexpr float armThick  = 0.5f;
        constexpr float wallH     = 3.0f;
        constexpr float halfArm   = armLength * 0.5f;

        float normRot = std::fmod(rotY, 360.0f);
        if (normRot < 0.0f) normRot += 360.0f;

        if (type == engine::PrefabType::Wall)
        {
            bool vertical = (normRot > 45.0f && normRot < 135.0f) ||
                            (normRot > 225.0f && normRot < 315.0f);

            if (vertical)
                SpawnColliderEntity(position, armThick, wallH, armLength);
            else
                SpawnColliderEntity(position, armLength, wallH, armThick);
        }
        else if (type == engine::PrefabType::WallCorner || type == engine::PrefabType::WallCornerSmall)
        {
            float dx1 = 0, dz1 = 0, dx2 = 0, dz2 = 0;

            int rot = static_cast<int>(normRot + 0.5f) % 360;
            if (rot < 45 || rot >= 315) {
                dx1 = -halfArm; dz1 = 0;
                dx2 = 0;        dz2 = halfArm;
            } else if (rot < 135) {
                dx1 = halfArm;  dz1 = 0;
                dx2 = 0;        dz2 = halfArm;
            } else if (rot < 225) {
                dx1 = halfArm;  dz1 = 0;
                dx2 = 0;        dz2 = -halfArm;
            } else {
                dx1 = -halfArm; dz1 = 0;
                dx2 = 0;        dz2 = -halfArm;
            }

            SpawnColliderEntity(position + glm::vec3(dx1, 0, 0), armLength, wallH, armThick);
            SpawnColliderEntity(position + glm::vec3(0, 0, dz2), armThick, wallH, armLength);
        }
        else if (type == engine::PrefabType::WallTsplit)
        {
            int rot = static_cast<int>(normRot + 0.5f) % 360;

            bool hasN = true, hasS = true, hasE = true, hasW = true;
            if (rot < 45 || rot >= 315)      hasN = false;
            else if (rot < 135)              hasW = false;
            else if (rot < 225)              hasS = false;
            else                             hasE = false;

            if (hasE && hasW)
                SpawnColliderEntity(position, armLength, wallH, armThick);

            if (hasN && hasS)
                SpawnColliderEntity(position, armThick, wallH, armLength);

            if (hasE && !hasW)
                SpawnColliderEntity(position + glm::vec3(halfArm, 0, 0), armLength, wallH, armThick);
            if (hasW && !hasE)
                SpawnColliderEntity(position + glm::vec3(-halfArm, 0, 0), armLength, wallH, armThick);
            if (hasS && !hasN)
                SpawnColliderEntity(position + glm::vec3(0, 0, halfArm), armThick, wallH, armLength);
            if (hasN && !hasS)
                SpawnColliderEntity(position + glm::vec3(0, 0, -halfArm), armThick, wallH, armLength);
        }
        else if (type == engine::PrefabType::WallCrossing)
        {
            SpawnColliderEntity(position, armLength, wallH, armThick);
            SpawnColliderEntity(position, armThick, wallH, armLength);
        }
    }

    void DungeonSpawnSystem::SpawnDoorwayPillarColliders(const glm::vec3& position, float rotY)
    {
        // The doorway scaffold has two pillars flanking the opening.
        // Pillars are ~0.5 wide/deep and sit ~1.5 units from center along the wall axis.
        constexpr float pillarThick = 0.5f;
        constexpr float pillarHeight = 3.0f;
        constexpr float pillarOffset = 1.5f;

        float normRot = std::fmod(rotY, 360.0f);
        if (normRot < 0.0f) normRot += 360.0f;

        bool vertical = (normRot > 45.0f && normRot < 135.0f) ||
                        (normRot > 225.0f && normRot < 315.0f);

        if (vertical)
        {
            // Wall runs along Z, pillars offset along Z
            SpawnColliderEntity(position + glm::vec3(0, 0, -pillarOffset), pillarThick, pillarHeight, pillarThick);
            SpawnColliderEntity(position + glm::vec3(0, 0, pillarOffset), pillarThick, pillarHeight, pillarThick);
        }
        else
        {
            // Wall runs along X, pillars offset along X
            SpawnColliderEntity(position + glm::vec3(-pillarOffset, 0, 0), pillarThick, pillarHeight, pillarThick);
            SpawnColliderEntity(position + glm::vec3(pillarOffset, 0, 0), pillarThick, pillarHeight, pillarThick);
        }
    }
}

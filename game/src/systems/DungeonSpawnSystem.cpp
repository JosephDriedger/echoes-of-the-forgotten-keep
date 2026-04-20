// Created by Adam Van Woerden

#include "game/systems/DungeonSpawnSystem.h"

#include "game/components/Components.h"

#include "engine/scene/BuildRoomSystem.h"
#include "engine/scene/FloorGenerator.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>

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

        m_LastMap = map;
        m_MapWorldWidth = map.width * config.buildConfig.tileSize;
        m_MapWorldDepth = map.height * config.buildConfig.tileSize;

        // Cache Start cell world position so GameplayScene can place the
        // player on top of the entry stairs (retry-based generation means
        // the Start cell is no longer at a predictable grid location).
        m_StartWorldPos = glm::vec3(0.0f);
        m_HasStartPos = false;
        for (int y = 0; y < map.height && !m_HasStartPos; y++) {
            for (int x = 0; x < map.width; x++) {
                if (map.cells[y * map.width + x] == engine::CellType::Start) {
                    const float ts = config.buildConfig.tileSize;
                    m_StartWorldPos = {
                        (x - map.width / 2.0f) * ts,
                        0.0f,
                        y * ts
                    };
                    m_HasStartPos = true;
                    break;
                }
            }
        }

        engine::BuildRoomSystem::DebugPrint(map);
        auto instances = engine::BuildRoomSystem::Build(map, config.buildConfig);

        for (const auto& inst : instances)
        {
            SpawnPrefab(inst.prefab, inst.position, inst.rotationY);

            // Below-ground stairs need extra wall pieces to seal the opening
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

        SpawnEnemies(map, config.buildConfig, seed);
        SpawnButtons(map, config.buildConfig, seed);
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

        // Bake the model matrix once. Dungeon geometry (walls/floors/stairs)
        // never moves, so precomputing spares the render loop a per-frame
        // translate+rotate+scale chain per entity. Door entities override this
        // flag when they start animating (see DoorSystem).
        if (type != engine::PrefabType::Door)
        {
            glm::mat4 m(1.0f);
            m = glm::translate(m, glm::vec3(t.X, t.Y, t.Z));
            m = glm::rotate(m, t.RotationY, glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(t.ScaleX, t.ScaleY, t.ScaleZ));
            t.ModelMatrix = m;
            t.UseModelMatrix = true;
        }

        m_Registry.AddComponent(e, t);
        m_Registry.AddComponent(e, Render(mesh.MeshPtr, texture));

        if (IsWallPrefab(type))
        {
            SpawnWallColliders(type, position, rotY);
        }

        if (type == engine::PrefabType::Stairs)
        {
            Collider col(5.0f, 4.5f, 4.0f);
            col.IsStatic = true;
            col.OffsetZ += 2;
            m_Registry.AddComponent(e, col);
        }

        if (type == engine::PrefabType::WallDoorway)
        {
            SpawnDoorwayPillarColliders(position, rotY);
        }

        if (type == engine::PrefabType::Door)
        {
            // The door hinge is offset from the doorway center. Shift the
            // trigger collider back toward the opening so the player walks
            // through the doorway, not past the hinge, to activate the door.
            float doorOffset = 0.82f;
            float normRot = std::fmod(rotY, 360.0f);
            if (normRot < 0.0f) normRot += 360.0f;

            Collider doorTrigger(2.5f, 3.0f, 2.5f);
            doorTrigger.IsTrigger = true;
            doorTrigger.IsStatic = false;

            // Choose offset axis based on rotation quadrant
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

            // Spawn a separate physical collider entity for the door panel
            engine::Entity doorCollider = m_Registry.CreateEntity();
            float baseRad = glm::radians(rotY);
            float halfPanel = door.PanelLength * 0.5f;
            float centerX = position.x + std::cos(baseRad) * halfPanel;
            float centerZ = position.z - std::sin(baseRad) * halfPanel;
            m_Registry.AddComponent(doorCollider, Transform(centerX, position.y, centerZ));

            // AABB sized to match door panel in its closed orientation
            float absS = std::abs(std::sin(baseRad));
            float absC = std::abs(std::cos(baseRad));
            float w = door.PanelLength * absC + door.PanelThickness * absS;
            float d = door.PanelLength * absS + door.PanelThickness * absC;
            Collider panelCol(w, 3.0f, d);
            panelCol.IsStatic = true;
            m_Registry.AddComponent(doorCollider, panelCol);

            door.ColliderEntity = doorCollider;
            m_Registry.AddComponent(e, door);

            SpawnedDoors.push_back(e);
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

    // Creates invisible AABB collider entities matching wall geometry.
    // Each wall type (straight, corner, T-split, crossing) gets collider
    // arms along its occupied axes based on the prefab's rotation.
    void DungeonSpawnSystem::SpawnWallColliders(engine::PrefabType type, const glm::vec3& position, float rotY)
    {
        constexpr float armLength = 4.0f;  // Length of one wall segment
        constexpr float armThick  = 1.2f;  // Wall thickness for collision
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
                dx1 = -halfArm+1; dz1 = 0;
                dx2 = 0;        dz2 = halfArm-1;
            } else if (rot < 135) {
                dx1 = halfArm-1;  dz1 = 0;
                dx2 = 0;        dz2 = halfArm-1;
            } else if (rot < 225) {
                dx1 = halfArm-1;  dz1 = 0;
                dx2 = 0;        dz2 = -halfArm+1;
            } else {
                dx1 = -halfArm+1; dz1 = 0;
                dx2 = 0;        dz2 = -halfArm+1;
            }

            SpawnColliderEntity(position + glm::vec3(dx1, 0, 0), halfArm, wallH, armThick);
            SpawnColliderEntity(position + glm::vec3(0, 0, dz2), armThick, wallH, halfArm);
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
                SpawnColliderEntity(position + glm::vec3(halfArm-1, 0, 0), halfArm, wallH, armThick);
            if (hasW && !hasE)
                SpawnColliderEntity(position + glm::vec3(-halfArm+1, 0, 0), halfArm, wallH, armThick);
            if (hasS && !hasN)
                SpawnColliderEntity(position + glm::vec3(0, 0, halfArm-1), armThick, wallH, halfArm);
            if (hasN && !hasS)
                SpawnColliderEntity(position + glm::vec3(0, 0, -halfArm+1), armThick, wallH, halfArm);
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
        constexpr float pillarThick = 1.2f;
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

    void DungeonSpawnSystem::SpawnEnemies(engine::MapGrid& map, const engine::BuildRoomConfig& config, int seed)
    {
        // Collect floor tile center positions (matching Build coordinate system)
        // Build uses: x_world = (x - map.width/2) * tileSize, z_world = y * tileSize
        // Floor tiles are placed at (x_world - floorOffset, floorY, z_world - floorOffset)
        // Enemy should stand at floor tile center: (x_world - floorOffset, 0, z_world - floorOffset)
        std::vector<glm::vec3> floorPositions;
        for (int y = 0; y < map.height; ++y)
        {
            for (int x = 0; x < map.width; ++x)
            {
                auto cell = map.get(x, y);
                // Only spawn on Floor tiles, skip Start/End/Door/Wall
                if (cell != engine::CellType::Floor) continue;

                // Check neighbors — skip tiles adjacent to walls to avoid clipping
                bool adjacentToWall = false;
                int dx[] = {-1, 1, 0, 0};
                int dz[] = {0, 0, -1, 1};
                for (int d = 0; d < 4; ++d)
                {
                    int nx = x + dx[d];
                    int nz = y + dz[d];
                    if (nx < 0 || nx >= map.width || nz < 0 || nz >= map.height)
                    {
                        adjacentToWall = true;
                        break;
                    }
                    auto neighbor = map.get(nx, nz);
                    if (neighbor == engine::CellType::Wall || neighbor == engine::CellType::Empty)
                    {
                        adjacentToWall = true;
                        break;
                    }
                }
                if (adjacentToWall) continue;

                float wx = (x - map.width / 2.0f) * config.tileSize - config.floorOffset;
                float wz = y * config.tileSize - config.floorOffset;
                floorPositions.emplace_back(wx, 0.0f, wz);
            }
        }

        if (floorPositions.empty()) return;

        std::mt19937 rng(seed);
        std::shuffle(floorPositions.begin(), floorPositions.end(), rng);

        // Cap enemy count (divisor of 1 means all eligible tiles, up to 100)
        int count = static_cast<int>(floorPositions.size());
        count = std::max(1, std::min(count, 100));

        // Load skeleton enemy mesh (randomly pick from available skeleton types)
        std::vector<std::string> skeletonModels = {
            "asset/characters/Skeleton_Minion.glb",
            // "asset/characters/Skeleton_Warrior.glb",
            // "asset/characters/Skeleton_Rogue.glb",
            // "asset/characters/Skeleton_Mage.glb"
        };
        auto texture = m_AssetManager.GetTextureManager().Load("asset/characters/skeleton_texture.png");

        if (!texture)
        {
            std::cerr << "[DungeonSpawnSystem] Failed to load skeleton texture\n";
            return;
        }

        std::uniform_int_distribution<int> modelDist(0, static_cast<int>(skeletonModels.size()) - 1);

        for (int i = 0; i < count && i < static_cast<int>(floorPositions.size()); ++i)
        {
            const auto& pos = floorPositions[i];

            // Pick a random skeleton model
            int modelIdx = modelDist(rng);
            auto meshResult = m_MeshManager.Load(skeletonModels[modelIdx]);

            if (!meshResult.MeshPtr)
            {
                std::cerr << "[DungeonSpawnSystem] Failed to load: " << skeletonModels[modelIdx] << "\n";
                continue;
            }

            // Store skeleton for animation (use first valid one)
            if (meshResult.SkeletonPtr && !EnemySkeleton)
                EnemySkeleton = meshResult.SkeletonPtr;

            engine::Entity e = m_Registry.CreateEntity();

            Transform t(pos.x, pos.y, pos.z);
            m_Registry.AddComponent(e, t);
            m_Registry.AddComponent(e, Render(meshResult.MeshPtr, texture));

            Collider col(1.2f, 1.8f, 1.2f);
            col.IsStatic = false;
            m_Registry.AddComponent(e, col);

            EnemyAI ai(2.0f);
            // Set up patrol around spawn point (±4 tiles in a random direction)
            std::uniform_real_distribution<float> offsetDist(-8.0f, 8.0f);
            float patrolOffX = offsetDist(rng);
            float patrolOffZ = offsetDist(rng);
            ai.HasPatrol = true;
            ai.PatrolAX = pos.x;
            ai.PatrolAZ = pos.z;
            ai.PatrolBX = pos.x - patrolOffX;
            ai.PatrolBZ = pos.z - patrolOffZ;
            m_Registry.AddComponent(e, ai);
            m_Registry.AddComponent(e, Health(3, 3));
            m_Registry.AddComponent(e, CombatState());

            // Add animation state if clips are available
            if (SharedClips && EnemySkeleton)
            {
                AnimationState anim;
                anim.SkeletonPtr = EnemySkeleton;
                anim.Clips = SharedClips;

                // Find skeleton-specific clip indices
                for (int c = 0; c < static_cast<int>(SharedClips->size()); ++c)
                {
                    const auto& name = (*SharedClips)[c].Name;
                    if (name == "Skeletons_Idle") anim.IdleClipIndex = c;
                    else if (name == "Skeletons_Walking") anim.RunClipIndex = c;
                    else if (name == "Skeletons_Death") anim.DeathClipIndex = c;
                    else if (name == "Melee_Unarmed_Attack_Punch_A") anim.Attack1ClipIndex = c;
                }

                // If no skeleton-specific clips, fall back to generic ones
                if (anim.IdleClipIndex < 0)
                {
                    for (int c = 0; c < static_cast<int>(SharedClips->size()); ++c)
                    {
                        const auto& name = (*SharedClips)[c].Name;
                        if (name == "Idle_A") anim.IdleClipIndex = c;
                        else if (name == "Running_A") anim.RunClipIndex = c;
                        else if (name == "Death_A") anim.DeathClipIndex = c;
                        else if (name == "Hit_A") anim.HitClipIndex = c;
                    }
                }

                if (anim.IdleClipIndex >= 0)
                    anim.CurrentClip = anim.IdleClipIndex;

                m_Registry.AddComponent(e, anim);
            }
        }

        std::cout << "[DungeonSpawnSystem] Spawned " << count << " enemies\n";
    }

    void DungeonSpawnSystem::SpawnButtons(engine::MapGrid& map, const engine::BuildRoomConfig& config, int seed)
    {
        // Collect interior floor positions (same filter as enemies)
        std::vector<glm::vec3> floorPositions;
        for (int y = 0; y < map.height; ++y)
        {
            for (int x = 0; x < map.width; ++x)
            {
                auto cell = map.get(x, y);
                if (cell != engine::CellType::Floor) continue;

                // Skip tiles next to walls
                bool adjacentToWall = false;
                int dx[] = {-1, 1, 0, 0};
                int dz[] = {0, 0, -1, 1};
                for (int d = 0; d < 4; ++d)
                {
                    int nx = x + dx[d];
                    int nz = y + dz[d];
                    if (nx < 0 || nx >= map.width || nz < 0 || nz >= map.height)
                    { adjacentToWall = true; break; }
                    auto neighbor = map.get(nx, nz);
                    if (neighbor == engine::CellType::Wall || neighbor == engine::CellType::Empty)
                    { adjacentToWall = true; break; }
                }
                if (adjacentToWall) continue;

                float wx = (x - map.width / 2.0f) * config.tileSize - config.floorOffset;
                float wz = y * config.tileSize - config.floorOffset;
                floorPositions.emplace_back(wx, 0.0f, wz);
            }
        }

        if (floorPositions.empty()) return;

        // Offset the RNG seed so button placement differs from enemy placement
        std::mt19937 rng(seed + 777);
        std::shuffle(floorPositions.begin(), floorPositions.end(), rng);

        // Spawn 1-2 buttons
        int buttonCount = std::min(2, static_cast<int>(floorPositions.size()));

        auto buttonMeshResult = m_MeshManager.Load("asset/dungeon/button_base_blue.gltf");
        auto buttonTexture = m_AssetManager.GetTextureManager().Load("asset/dungeon/platformer_texture.png");

        if (!buttonMeshResult.MeshPtr || !buttonTexture)
        {
            std::cerr << "[DungeonSpawnSystem] Failed to load button mesh\n";
            return;
        }

        // Only spawn as many buttons as we have doors to link to
        buttonCount = std::min(buttonCount, static_cast<int>(SpawnedDoors.size()));

        for (int i = 0; i < buttonCount; ++i)
        {
            const auto& pos = floorPositions[i];

            engine::Entity e = m_Registry.CreateEntity();

            Transform t(pos.x, pos.y, pos.z);
            m_Registry.AddComponent(e, t);
            m_Registry.AddComponent(e, Render(buttonMeshResult.MeshPtr, buttonTexture));

            Collider col(1.5f, 0.5f, 1.5f);
            col.IsTrigger = true;
            col.IsStatic = true;
            m_Registry.AddComponent(e, col);

            std::string switchId = "Button_" + std::to_string(i);
            m_Registry.AddComponent(e, Switch(switchId));

            // Link a door to this button
            engine::Entity doorEntity = SpawnedDoors[i];
            if (m_Registry.IsAlive(doorEntity) && m_Registry.HasComponent<Door>(doorEntity))
            {
                auto& door = m_Registry.GetComponent<Door>(doorEntity);
                // door.TriggerId = switchId;
                door.SwingDirection = 1; // always open one direction for puzzle doors
            }
        }

        std::cout << "[DungeonSpawnSystem] Spawned " << buttonCount << " buttons\n";
    }

    void DungeonSpawnSystem::DebugPrintMap() const
    {
        engine::BuildRoomSystem::DebugPrint(m_LastMap);
    }
}

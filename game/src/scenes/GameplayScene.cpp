#include "game/scenes/GameplayScene.h"

#include "engine/core/Application.h"
#include "engine/rendering/Renderer.h"
#include "engine/rendering/Shader.h"

#include "game/systems/EntitySpawnSystem.h"
#include "game/systems/MovementSystem.h"
#include "game/systems/AnimationSystem.h"
#include "game/systems/BoneAttachmentSystem.h"
#include "game/components/Transform.h"
#include "game/components/Render.h"
#include "game/components/AnimationState.h"
#include "game/components/BoneAttachment.h"
#include "game/components/Player.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>
#include <SDL3/SDL_keycode.h>

#include <cmath>
#include <iostream>

#include "engine/scene/BuildRoomSystem.h"
#include "engine/scene/FloorGenerator.h"

namespace game
{
    GameplayScene::GameplayScene()
        : engine::Scene("GameplayScene"),
          m_Camera(),
          m_RenderSystem(),
          m_AssetManager(),
          m_MeshManager(),
          m_AnimationLoader(),
          m_Registry(),
          m_PlayerEntity(0)
    {
    }

    bool GameplayScene::OnCreate(engine::Application& application)
    {
        m_Camera.SetPosition(0.0f, 10.0f, 13.0f);
        m_Camera.SetTarget(0.0f, 0.0f, 5.0f);
        // m_Camera.SetOrthographic(-8.0f, 8.0f, -4.5f, 4.5f, 0.1f, 100.0f);
        m_Camera.SetPerspective(40.0f,16.0f / 9.0f,0.1f,100.0f
);

        m_Shader = m_AssetManager.GetShaderManager().Load(
            "default",
            "asset/shaders/vertex.glsl",
            "asset/shaders/fragment.glsl");

        if (!m_Shader)
        {
            std::cerr << "[GameplayScene] Failed to load shader\n";
            return false;
        }

        application.GetRenderer().SetActiveCamera(&m_Camera);

        EntitySpawnSystem::Register(m_Registry);

        LoadContent();
        InitializeScene();

        return true;
    }

    void GameplayScene::OnDestroy()
    {
        m_Registry.Reset();
        m_MeshManager.Clear();
        m_AssetManager.Clear();
        m_PlayerClips.reset();
        m_PlayerSkeleton.reset();
    }

    void GameplayScene::LoadContent()
    {
        // Load player mesh + skeleton from mesh file (bone IDs match vertex data)
        std::string playerMeshPath = "asset/Knight.glb";
        engine::MeshLoader::Result playerResult = m_MeshManager.Load(playerMeshPath);

        if (playerResult.SkeletonPtr)
        {
            m_PlayerSkeleton = playerResult.SkeletonPtr;
        }

        // Load animation clips
        m_PlayerClips = std::make_shared<std::vector<engine::AnimationClip>>();

        std::vector<std::string> animPaths = {
            "asset/animations/Rig_Medium_General.glb",
            "asset/animations/Rig_Medium_MovementBasic.glb",
            "asset/animations/Rig_Medium_MovementAdvanced.glb",
            "asset/animations/Rig_Medium_Simulation.glb",
            "asset/animations/Rig_Medium_Special.glb",
            "asset/animations/Rig_Medium_Tools.glb",
            "asset/animations/Rig_Medium_CombatMelee.glb",
            "asset/animations/Rig_Medium_CombatRanged.glb"
        };

        for (const auto& path : animPaths)
        {
            engine::AnimationLoader::AppendClips(path, *m_PlayerClips, m_AnimationLoader);
        }

        std::cout << "[GameplayScene] Loaded " << m_PlayerClips->size() << " animation clips\n";
        for (int i = 0; i < static_cast<int>(m_PlayerClips->size()); i++)
        {
            std::cout << "  [" << i << "] " << (*m_PlayerClips)[i].Name << "\n";
        }
        RegisterPrefab();
    }

    void GameplayScene::RegisterPrefab() {
        const std::string dungeonTexPath = "asset/dungeon/dungeon_texture.png";
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::WallCorner,
            {"asset/dungeon/wall_corner.gltf",dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::WallCrossing,
            {"asset/dungeon/wall_crossing.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::WallTsplit,
            {"asset/dungeon/wall_Tsplit.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::WallDoorway,
            {"asset/dungeon/wall_doorway_scaffold.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::Door,
            {"asset/dungeon/door.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::FloorLarge,
            {"asset/dungeon/floor_tile_large.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::FloorSmall,
            {"asset/dungeon/floor_tile_small.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::Stairs,
{"asset/dungeon/stairs_walled.gltf", dungeonTexPath});

        engine::PrefabManager::Register(engine::PrefabType::WallCornerSmall,
{"asset/dungeon/wall_corner_small.gltf", dungeonTexPath});
    }

    int GameplayScene::FindClipIndex(const std::string& name) const
    {
        if (!m_PlayerClips) return -1;

        for (int i = 0; i < static_cast<int>(m_PlayerClips->size()); i++)
        {
            if ((*m_PlayerClips)[i].Name == name)
                return i;
        }

        std::cerr << "[GameplayScene] Clip not found: " << name << "\n";
        return -1;
    }

    void GameplayScene::InitializeScene()
    {
        // Spawn player
        m_PlayerEntity = EntitySpawnSystem::SpawnPlayer(m_Registry);
        auto& playerTransform = m_Registry.GetComponent<Transform>(m_PlayerEntity);
        playerTransform.X = 32.0f;
        playerTransform.Y = 0.0f;
        playerTransform.Z = 25.0f;

        // Add render component
        auto playerMesh = m_MeshManager.Get("asset/Knight.glb");
        auto playerTexture = m_AssetManager.GetTextureManager().Load("asset/knight_texture.png");
        m_Registry.AddComponent(m_PlayerEntity, Render(playerMesh, playerTexture));

        // Add animation state
        AnimationState animState;
        animState.SkeletonPtr = m_PlayerSkeleton;
        animState.Clips = m_PlayerClips;

        animState.IdleClipIndex = FindClipIndex("Idle_A");
        animState.RunClipIndex = FindClipIndex("Running_A");
        animState.Attack1ClipIndex = FindClipIndex("Melee_1H_Attack_Slice_Horizontal");
        animState.Attack2ClipIndex = FindClipIndex("Melee_1H_Attack_Stab");
        animState.Attack3ClipIndex = FindClipIndex("Melee_1H_Attack_Jump_Chop");
        animState.HitClipIndex = FindClipIndex("Hit_A");
        animState.DeathClipIndex = FindClipIndex("Death_A");

        if (animState.IdleClipIndex >= 0)
            animState.CurrentClip = animState.IdleClipIndex;

        m_Registry.AddComponent(m_PlayerEntity, animState);

        // Spawn sword (right hand)
        {
            engine::MeshLoader::Result swordResult = m_MeshManager.Load("asset/sword_1handed.gltf");
            auto swordTexture = m_AssetManager.GetTextureManager().Load("asset/knight_texture.png");

            if (swordResult.MeshPtr && swordTexture)
            {
                engine::Entity swordEntity = m_Registry.CreateEntity();
                m_Registry.AddComponent(swordEntity, Transform(0.0f, 0.0f, 0.0f));
                m_Registry.AddComponent(swordEntity, Render(swordResult.MeshPtr, swordTexture));
                m_Registry.AddComponent(swordEntity,
                    BoneAttachment(m_PlayerEntity, "handslot.r", glm::mat4(1.0f)));
            }
        }

        // Spawn shield (left hand)
        {
            engine::MeshLoader::Result shieldResult = m_MeshManager.Load("asset/shield_spikes.gltf");
            auto shieldTexture = m_AssetManager.GetTextureManager().Load("asset/knight_texture.png");

            if (shieldResult.MeshPtr && shieldTexture)
            {
                engine::Entity shieldEntity = m_Registry.CreateEntity();
                m_Registry.AddComponent(shieldEntity, Transform(0.0f, 0.0f, 0.0f));
                m_Registry.AddComponent(shieldEntity, Render(shieldResult.MeshPtr, shieldTexture));
                m_Registry.AddComponent(shieldEntity,
                    BoneAttachment(m_PlayerEntity, "handslot.l",
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.15f))));
            }
        }

        // Dungeon room
        engine::FloorConfig config;
        config.roomCount = 5;
        config.seed = 42;
        config.mazeFactor = 0.5f;

        auto dungeon = engine::FloorGenerator::Generate(config);

        // Convert to MapGrid (your existing system)
        auto map = engine::BuildRoomSystem::FromFloor(dungeon);

        engine::BuildRoomSystem::DebugPrint(map);
        auto instances = engine::BuildRoomSystem::Build(map, config.buildConfig);

        for (const auto& inst : instances) {
            SpawnPrefab(inst.prefab, inst.position, inst.rotationY);
            if (inst.prefab == engine::PrefabType::Stairs && inst.position.y < -1) {
                SpawnPrefab(engine::PrefabType::WallCornerSmall, inst.position + glm::vec3 {2,0,4.5}, inst.rotationY-90);
                SpawnPrefab(engine::PrefabType::Wall, inst.position + glm::vec3 {0,0,4.5}, inst.rotationY);
                SpawnPrefab(engine::PrefabType::WallCornerSmall, inst.position + glm::vec3 {-2,0,4.5}, inst.rotationY+180);
            }
        }

        // // North walls
        // SpawnDungeonPiece("asset/dungeon/wall_corner.gltf", dungeonTexPath, -6, 0, 0, 90);
        // SpawnDungeonPiece("asset/dungeon/wall.gltf", dungeonTexPath, -2, 0, 0, 0);
        // SpawnDungeonPiece("asset/dungeon/wall_doorway_scaffold.gltf", dungeonTexPath, 2, 0, 0, 0);
        // SpawnDungeonPiece("asset/dungeon/door.gltf", dungeonTexPath, 1.18f, 0, 0, 0);
        // SpawnDungeonPiece("asset/dungeon/wall_corner.gltf", dungeonTexPath, 6, 0, 0, 0);
        //
        // // East walls
        // SpawnDungeonPiece("asset/dungeon/wall.gltf", dungeonTexPath, 6, 0, 2, 90);
        // SpawnDungeonPiece("asset/dungeon/wall.gltf", dungeonTexPath, 6, 0, 6, 90);
        //
        // // West walls
        // SpawnDungeonPiece("asset/dungeon/wall.gltf", dungeonTexPath, -6, 0, 2, 90);
        // SpawnDungeonPiece("asset/dungeon/wall.gltf", dungeonTexPath, -6, 0, 6, 90);
        //
        // // South walls
        // SpawnDungeonPiece("asset/dungeon/wall_corner.gltf", dungeonTexPath, -6, 0, 10, 180);
        // SpawnDungeonPiece("asset/dungeon/wall.gltf", dungeonTexPath, -2, 0, 10, 0);
        // SpawnDungeonPiece("asset/dungeon/wall_doorway_scaffold.gltf", dungeonTexPath, 2, 0, 10, 0);
        // SpawnDungeonPiece("asset/dungeon/door.gltf", dungeonTexPath, 1.18f, 0, 10, 0);
        // SpawnDungeonPiece("asset/dungeon/wall_corner.gltf", dungeonTexPath, 6, 0, 10, -90);
        //
        // // Floor tiles (large)
        // SpawnDungeonPiece("asset/dungeon/floor_tile_large.gltf", dungeonTexPath, -4, -0.1f, 2, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_large.gltf", dungeonTexPath, 0, -0.1f, 2, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_large.gltf", dungeonTexPath, 4, -0.1f, 2, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_large.gltf", dungeonTexPath, -4, -0.1f, 6, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_large.gltf", dungeonTexPath, 0, -0.1f, 6, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_large.gltf", dungeonTexPath, 4, -0.1f, 6, 0);
        //
        // // Floor tiles (small)
        // SpawnDungeonPiece("asset/dungeon/floor_tile_small.gltf", dungeonTexPath, -5, -0.1f, 9, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_small.gltf", dungeonTexPath, -3, -0.1f, 9, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_small.gltf", dungeonTexPath, -1, -0.1f, 9, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_small.gltf", dungeonTexPath, 1, -0.1f, 9, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_small.gltf", dungeonTexPath, 3, -0.1f, 9, 0);
        // SpawnDungeonPiece("asset/dungeon/floor_tile_small.gltf", dungeonTexPath, 5, -0.1f, 9, 0);
    }

    engine::Entity GameplayScene::SpawnPrefab(
    engine::PrefabType type,
    const glm::vec3& position,
    const float rotY)
    {
        const auto* def = engine::PrefabManager::GetRandom(type);
        if (!def) {
            std::cerr << "[SpawnPrefab] Missing prefab\n";
            return engine::Entity(0);
        }

        auto mesh = m_MeshManager.Load(def->meshPath);
        auto texture = m_AssetManager.GetTextureManager().Load(def->texturePath);

        if (!mesh.MeshPtr || !texture) {
            std::cerr << "[SpawnPrefab] Failed to load: " << def->meshPath << "\n";
            return engine::Entity(0);
        }

        engine::Entity e = m_Registry.CreateEntity();

        Transform t(position.x, position.y, position.z);
        t.RotationY = glm::radians(rotY);

        m_Registry.AddComponent(e, t);
        m_Registry.AddComponent(e, Render(mesh.MeshPtr, texture));

        return e;
    }

    void GameplayScene::OnUpdate(engine::Application& application, const engine::Timestep timestep)
    {
        const float dt = timestep.GetSeconds();
        const engine::Input& input = application.GetInput();

        if (input.IsKeyPressed(SDLK_ESCAPE))
        {
            application.RequestQuit();
        }

        // Attack input
        if (input.IsKeyPressed(SDLK_SPACE))
        {
            if (m_Registry.HasComponent<AnimationState>(m_PlayerEntity))
            {
                auto& anim = m_Registry.GetComponent<AnimationState>(m_PlayerEntity);
                if (!anim.IsDead)
                {
                    if (anim.IsAttacking)
                    {
                        if (anim.ComboTimer > 0.0f && anim.ComboTimer <= anim.ComboWindow)
                        {
                            anim.AttackQueued = true;
                        }
                    }
                    else
                    {
                        anim.IsAttacking = true;
                        anim.ComboIndex = 0;
                        anim.CurrentTime = 0.0f;
                        if (anim.Attack1ClipIndex >= 0)
                            anim.CurrentClip = anim.Attack1ClipIndex;
                    }
                }
            }
        }

        MovementSystem::Update(m_Registry, input, dt);
        AnimationSystem::Update(m_Registry, dt);
        BoneAttachmentSystem::Update(m_Registry);

        UpdateCamera();
    }

    void GameplayScene::UpdateCamera()
    {
        if (!m_Registry.IsAlive(m_PlayerEntity))
            return;

        const auto& transform = m_Registry.GetComponent<Transform>(m_PlayerEntity);
        m_Camera.SetPosition(transform.X, 20.0f, transform.Z + 8.0f);
        m_Camera.SetTarget(transform.X, 0.0f, transform.Z);
    }

    void GameplayScene::OnRender(engine::Application& application)
    {
        (void)application;

        if (!m_Shader)
            return;

        m_Shader->Bind();
        m_Shader->SetMat4("view", m_Camera.GetViewMatrix());
        m_Shader->SetMat4("projection", m_Camera.GetProjectionMatrix());
        m_Shader->SetInt("texture_diffuse1", 0);

        for (const engine::Entity entity : m_Registry.GetActiveEntities())
        {
            if (!m_Registry.HasComponent<Transform>(entity) ||
                !m_Registry.HasComponent<Render>(entity))
            {
                continue;
            }

            const auto& transform = m_Registry.GetComponent<Transform>(entity);
            const auto& render = m_Registry.GetComponent<Render>(entity);

            if (!render.MeshPtr || !render.TexturePtr)
                continue;

            // Build model matrix
            glm::mat4 modelMat;
            if (transform.UseModelMatrix)
            {
                modelMat = transform.ModelMatrix;
            }
            else
            {
                modelMat = glm::mat4(1.0f);
                modelMat = glm::translate(modelMat, glm::vec3(transform.X, transform.Y, transform.Z));
                modelMat = glm::rotate(modelMat, transform.RotationY, glm::vec3(0.0f, 1.0f, 0.0f));
                modelMat = glm::scale(modelMat, glm::vec3(transform.ScaleX, transform.ScaleY, transform.ScaleZ));
            }

            m_Shader->SetMat4("model", glm::value_ptr(modelMat));

            // Upload bone matrices
            bool hasAnimation = m_Registry.HasComponent<AnimationState>(entity);
            if (hasAnimation)
            {
                const auto& anim = m_Registry.GetComponent<AnimationState>(entity);
                for (int i = 0; i < static_cast<int>(anim.FinalBoneMatrices.size()); i++)
                {
                    std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
                    m_Shader->SetMat4(name.c_str(), glm::value_ptr(anim.FinalBoneMatrices[i]));
                }
            }
            else
            {
                glm::mat4 identity(1.0f);
                for (int i = 0; i < 100; i++)
                {
                    std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
                    m_Shader->SetMat4(name.c_str(), glm::value_ptr(identity));
                }
            }

            render.TexturePtr->Bind(0);
            render.MeshPtr->Draw();
        }
    }
}

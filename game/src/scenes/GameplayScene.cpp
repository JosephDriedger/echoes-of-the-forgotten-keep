// Created by Adam Van Woerden

#include "game/scenes/GameplayScene.h"

#include "engine/core/Application.h"
#include "engine/platform/Window.h"
#include "engine/rendering/Renderer.h"
#include "engine/rendering/Shader.h"

#include "game/systems/EntitySpawnSystem.h"
#include "game/systems/MovementSystem.h"
#include "game/systems/AnimationSystem.h"
#include "game/systems/BoneAttachmentSystem.h"
#include "game/systems/CombatInputSystem.h"
#include "game/systems/DoorSystem.h"
#include "game/systems/DungeonSpawnSystem.h"
#include "game/components/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>

#include <iostream>

namespace game
{
    GameplayScene::GameplayScene()
        : engine::Scene("GameplayScene"),
          m_Camera(),
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
        m_Camera.SetPerspective(40.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        m_UISystem.Initialize(m_AssetManager);

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

        if (!m_DebugColliderRenderer.Initialize())
        {
            std::cerr << "[GameplayScene] Failed to initialize debug collider renderer\n";
        }

        return true;
    }

    void GameplayScene::OnDestroy()
    {
        m_Registry.Reset();
        m_MeshManager.Clear();
        m_AssetManager.Clear();
        m_PlayerClips.reset();
        m_PlayerSkeleton.reset();
        m_DungeonSpawnSystem.reset();
    }

    void GameplayScene::LoadContent()
    {
        std::string playerMeshPath = "asset/characters/Knight.glb";
        engine::MeshLoader::Result playerResult = m_MeshManager.Load(playerMeshPath);

        if (playerResult.SkeletonPtr)
        {
            m_PlayerSkeleton = playerResult.SkeletonPtr;
        }

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

        RegisterPrefabs();
    }

    void GameplayScene::RegisterPrefabs()
    {
        const std::string dungeonTexPath = "asset/dungeon/dungeon_texture.png";
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall.gltf", dungeonTexPath});
        engine::PrefabManager::Register(engine::PrefabType::WallCorner,
            {"asset/dungeon/wall_corner.gltf", dungeonTexPath});
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

        auto& playerCollider = m_Registry.GetComponent<Collider>(m_PlayerEntity);
        playerCollider.Width = 1.2f;
        playerCollider.Height = 1.8f;
        playerCollider.Depth = 1.2f;
        playerCollider.IsStatic = false;

        // Add render component
        auto playerMesh = m_MeshManager.Get("asset/characters/Knight.glb");
        auto playerTexture = m_AssetManager.GetTextureManager().Load("asset/characters/knight_texture.png");
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

        // Add combat state and health
        m_Registry.AddComponent(m_PlayerEntity, CombatState());
        m_Registry.AddComponent(m_PlayerEntity, Health(10, 10));

        // Spawn sword (right hand)
        {
            engine::MeshLoader::Result swordResult = m_MeshManager.Load("asset/equipment/sword_1handed.gltf");
            auto swordTexture = m_AssetManager.GetTextureManager().Load("asset/characters/knight_texture.png");

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
            engine::MeshLoader::Result shieldResult = m_MeshManager.Load("asset/equipment/shield_spikes.gltf");
            auto shieldTexture = m_AssetManager.GetTextureManager().Load("asset/characters/knight_texture.png");

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

        // Spawn dungeon via DungeonSpawnSystem
        m_DungeonSpawnSystem = std::make_unique<DungeonSpawnSystem>(
            m_Registry, m_MeshManager, m_AssetManager);
        m_DungeonSpawnSystem->SharedClips = m_PlayerClips;
        m_DungeonSpawnSystem->SpawnDungeon(5, 42, 0.5f);
    }

    void GameplayScene::OnUpdate(engine::Application& application, const engine::Timestep timestep)
    {
        const float dt = timestep.GetSeconds();
        const engine::Input& input = application.GetInput();

        // Debug systems
        m_DebugToggle.Update(input);
        m_FPSCounter.Update(timestep);

        if (m_DebugToggle.ShowFPS())
        {
            std::string title = "Echoes of the Forgotten Keep - " + m_FPSCounter.GetDisplayString();
            if (engine::Window* window = application.GetWindow())
                SDL_SetWindowTitle(window->GetNativeWindow(), title.c_str());
        }
        else
        {
            if (engine::Window* window = application.GetWindow())
                SDL_SetWindowTitle(window->GetNativeWindow(), "Echoes of the Forgotten Keep");
        }

        if (input.IsKeyPressed(SDLK_ESCAPE))
        {
            application.RequestSceneChange("PauseMenuScene");
            return;
        }

        // ECS systems
        CombatInputSystem::Update(m_Registry, input);
        MovementSystem::Update(m_Registry, input, dt);
        m_CollisionSystem.Update(m_Registry);
        AnimationSystem::Update(m_Registry, dt);
        BoneAttachmentSystem::Update(m_Registry);
        m_AttackHitboxSystem.Update(m_Registry);
        m_EnemyAISystem.Update(m_Registry, dt);
        m_CombatSystem.Update(m_Registry, dt);
        m_DamageSystem.Update(m_Registry);
        SwitchTriggerSystem::Update(m_Registry);
        DoorSystem::Update(m_Registry, dt);
        DoorPuzzleSystem::Update(m_Registry, dt);
        LifetimeSystem::Update(m_Registry, dt);
        m_CameraFollowSystem.Update(m_Registry, m_PlayerEntity, m_Camera, input);
    }

    void GameplayScene::OnRender(engine::Application& application)
    {
        (void)application;

        if (!m_Shader)
            return;

        // Pre-cache bone uniform locations on first render
        static std::vector<int> boneUniformLocations;
        if (boneUniformLocations.empty())
        {
            boneUniformLocations.resize(100);
            for (int i = 0; i < 100; i++)
            {
                std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
                boneUniformLocations[i] = m_Shader->GetCachedUniformLocation(name);
            }
        }

        m_Shader->Bind();
        m_Shader->SetMat4("view", m_Camera.GetViewMatrix());
        m_Shader->SetMat4("projection", m_Camera.GetProjectionMatrix());
        m_Shader->SetInt("texture_diffuse1", 0);

        // Upload identity bones once at start for non-animated entities
        static bool identityUploaded = false;
        if (!identityUploaded)
        {
            glm::mat4 identity(1.0f);
            for (int i = 0; i < 100; i++)
            {
                m_Shader->SetMat4(boneUniformLocations[i], glm::value_ptr(identity));
            }
            identityUploaded = true;
        }

        bool lastWasAnimated = false;

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
                    m_Shader->SetMat4(boneUniformLocations[i], glm::value_ptr(anim.FinalBoneMatrices[i]));
                }
                lastWasAnimated = true;
            }
            else if (lastWasAnimated)
            {
                glm::mat4 identity(1.0f);
                for (int i = 0; i < 100; i++)
                {
                    m_Shader->SetMat4(boneUniformLocations[i], glm::value_ptr(identity));
                }
                lastWasAnimated = false;
            }

            render.TexturePtr->Bind(0);
            render.MeshPtr->Draw();
        }

        // Render debug colliders after main scene
        if (m_DebugToggle.ShowColliders())
        {
            m_DebugColliderRenderer.Render(m_Registry, m_Camera);
        }

        int w, h;
        SDL_GetWindowSize(application.GetWindow()->GetNativeWindow(), &w, &h);
        m_UISystem.Render(m_Registry, m_PlayerEntity, w, h);
    }
}

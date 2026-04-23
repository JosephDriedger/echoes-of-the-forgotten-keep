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
#include "game/systems/DeathSystem.h"
#include "game/systems/DoorSystem.h"
#include "game/systems/DungeonSpawnSystem.h"
#include "game/components/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>

#include <algorithm>
#include <iostream>

#include "game/systems/DoorPuzzleSystem.h"

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
        // Set up the third-person camera with a top-down offset
        m_Camera.SetPosition(0.0f, 10.0f, 13.0f);
        m_Camera.SetTarget(0.0f, 0.0f, 5.0f);
        m_Camera.SetPerspective(40.0f, 16.0f / 9.0f, 0.1f, 600.0f);
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

        const auto& spec = application.GetSpecification();
        m_DebugTextRenderer.Initialize(
            "asset/fonts/BreatheFireIi-2z9W.ttf", 32.0f,
            "asset/shaders/text_vertex.glsl",
            "asset/shaders/text_fragment.glsl",
            spec.Width, spec.Height);

        return true;
    }

    /// Tears down all scene resources so the scene can be recreated cleanly.
    void GameplayScene::OnDestroy()
    {
        m_Registry.Reset();
        m_MeshManager.Clear();
        m_AssetManager.Clear();
        m_PlayerClips.reset();
        m_PlayerSkeleton.reset();
        m_DungeonSpawnSystem.reset();
        m_DebugTextRenderer.Destroy();
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
        engine::PrefabManager::Register(engine::PrefabType::FloorLarge,
            {"asset/dungeon/floor_tile_big_grate.gltf", dungeonTexPath, 0.05});
        engine::PrefabManager::Register(engine::PrefabType::FloorLarge,
            {"asset/dungeon/floor_tile_large_rocks.gltf", dungeonTexPath, 0.1});
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall_broken.gltf", dungeonTexPath, 0.01});
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall_pillar.gltf", dungeonTexPath, 0.1});
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall_shelves.gltf", dungeonTexPath, 0.01});
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall_scaffold.gltf", dungeonTexPath, 0.1});
        engine::PrefabManager::Register(engine::PrefabType::Wall,
            {"asset/dungeon/wall_window_closed.gltf", dungeonTexPath, 0.1});
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
        animState.AnimationSpeed = 1.5f;

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

        // Place player at the dungeon's start cell
        if (m_DungeonSpawnSystem->HasStartPos())
        {
            const glm::vec3& start = m_DungeonSpawnSystem->GetStartWorldPos();
            auto& t = m_Registry.GetComponent<Transform>(m_PlayerEntity);
            t.X = start.x;
            t.Y = start.y;
            t.Z = start.z;
        }
    }

    /// Runs the full ECS pipeline each frame. Order matters: input -> movement
    /// -> collision -> animation -> combat -> AI -> doors -> lifetime -> camera.
    void GameplayScene::OnUpdate(engine::Application& application, const engine::Timestep timestep)
    {
        const float dt = timestep.GetSeconds();
        const engine::Input& input = application.GetInput();

        m_DebugToggle.Update(input);
        m_FPSCounter.Update(timestep);

        // Push pause overlay on ESC
        if (input.IsKeyPressed(SDLK_ESCAPE))
        {
            application.RequestSceneChange("PauseMenuScene");
            return;
        }

        // ECS systems
        CombatInputSystem::Update(m_Registry, input, audioEventQueue);
        MovementSystem::Update(m_Registry, input, dt);
        CollisionSystem::Update(m_Registry);
        AnimationSystem::Update(m_Registry, dt, audioEventQueue);
        BoneAttachmentSystem::Update(m_Registry);
        AttackHitboxSystem::Update(m_Registry);
        DamageSystem::Update(m_Registry);
        DeathSystem::Update(m_Registry, audioEventQueue);
        EnemyAISystem::Update(m_Registry, dt);
        HitTimerSystem::Update(m_Registry, dt);
        SwitchTriggerSystem::Update(m_Registry);
        DoorSystem::Update(m_Registry, dt, audioEventQueue);
        DoorPuzzleSystem::Update(m_Registry, dt);
        LifetimeSystem::Update(m_Registry, dt);
        CameraFollowSystem::Update(m_Registry, m_PlayerEntity, m_Camera, input);

        // Debug: top-down overview of the entire dungeon (F6). Overrides
        // the follow camera so we can see the whole generated map at once.
        // Also dumps the ASCII grid to stdout each time it's toggled.
        if (m_DebugToggle.ShowMapOverview() && m_DungeonSpawnSystem)
        {
            const float mapDepth = m_DungeonSpawnSystem->GetMapWorldDepth();
            const float mapWidth = m_DungeonSpawnSystem->GetMapWorldWidth();
            const float centerZ = mapDepth * 0.5f;
            // Height chosen so a 40° vertical FOV comfortably covers the map.
            const float height = std::max(mapWidth, mapDepth) * 1.6f;
            m_Camera.SetPosition(0.0f, height, centerZ + 20.0f);
            m_Camera.SetTarget(0.0f, 0.0f, centerZ);
        }
        if (m_DebugToggle.MapOverviewJustToggled() && m_DungeonSpawnSystem)
        {
            m_DungeonSpawnSystem->DebugPrintMap();
        }

        audioEventQueue.process(); // process all the audio events
    }

    /// Renders all entities that have Transform + Render components, uploads
    /// per-entity bone matrices for animated meshes, then draws debug overlays and HUD.
    void GameplayScene::OnRender(engine::Application& application)
    {
        (void)application;

        if (!m_Shader)
            return;

        // Cache bone uniform locations once to avoid repeated string lookups
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

        // Treat the start of each frame as if the previous entity was animated so the
        // first non-animated entity always gets identity bones, regardless of draw order.
        bool lastWasAnimated = true;

        // Track last-bound GPU state so we can skip redundant glBindTexture /
        // glBindVertexArray calls when consecutive entities share resources
        // (common in the dungeon where dozens of wall/floor tiles share
        // meshes and textures).
        unsigned int lastTextureId = 0;
        unsigned int lastVaoId = 0;

        // Distance cull: skip entities that are far enough from the camera
        // that they can't possibly be on screen. 80 units squared radius
        // comfortably covers the 3rd-person view; the overview camera (F6)
        // sits well above this so we disable culling when it's active.
        const bool useCull = !m_DebugToggle.ShowMapOverview();
        const float cullRadiusSq = 80.0f * 80.0f;
        const float camX = m_Camera.GetPositionX();
        const float camZ = m_Camera.GetPositionZ();

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

            // Bone-attached entities (sword, shield, etc.) carry a stale
            // (0,0,0) transform -- BoneAttachmentSystem writes their real
            // world position into ModelMatrix each frame. Skip distance
            // culling for them so equipment riding on the player doesn't
            // get culled when the player roams far from the origin.
            if (useCull && !m_Registry.HasComponent<BoneAttachment>(entity))
            {
                const float dx = transform.X - camX;
                const float dz = transform.Z - camZ;
                if (dx * dx + dz * dz > cullRadiusSq)
                    continue;
            }

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

            const unsigned int texId = render.TexturePtr->GetId();
            if (texId != lastTextureId)
            {
                render.TexturePtr->Bind(0);
                lastTextureId = texId;
            }

            // DrawBatched compares against lastVaoId and only calls
            // glBindVertexArray when the mesh actually changes.
            render.MeshPtr->DrawBatched(lastVaoId);
        }

        // Render debug colliders after main scene
        if (m_DebugToggle.ShowColliders())
        {
            m_DebugColliderRenderer.Render(m_Registry, m_Camera);
        }

        int w, h;
        SDL_GetWindowSize(application.GetWindow()->GetNativeWindow(), &w, &h);
        m_UISystem.Render(m_Registry, m_PlayerEntity, w, h);

        // Debug FPS overlay (top-right corner)
        if (m_DebugToggle.ShowFPS())
        {
            const std::string& fps = m_FPSCounter.GetDisplayString();
            float scale = 0.5f;
            float textW = m_DebugTextRenderer.MeasureTextWidth(fps, scale);
            m_DebugTextRenderer.RenderText(fps,
                static_cast<float>(w) - textW - 10.0f, 10.0f,
                scale, 0.0f, 1.0f, 0.0f);
        }
    }
}

#include "game/scenes/GameplayScene.h"

#include "engine/core/Application.h"
#include "engine/platform/Window.h"
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
#include "game/components/Collider.h"
#include "game/components/Door.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL.h>

#include <cmath>
#include <iostream>

#include "engine/scene/BuildRoomSystem.h"
#include "engine/scene/FloorGenerator.h"

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

        // Initialize debug collider renderer
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

    bool GameplayScene::IsWallPrefab(engine::PrefabType type)
    {
        return type == engine::PrefabType::Wall ||
               type == engine::PrefabType::WallCorner ||
               type == engine::PrefabType::WallCrossing ||
               type == engine::PrefabType::WallTsplit ||
               type == engine::PrefabType::WallCornerSmall;
    }

    void GameplayScene::InitializeScene()
    {
        // Spawn player
        m_PlayerEntity = EntitySpawnSystem::SpawnPlayer(m_Registry);
        auto& playerTransform = m_Registry.GetComponent<Transform>(m_PlayerEntity);
        playerTransform.X = 32.0f;
        playerTransform.Y = 0.0f;
        playerTransform.Z = 25.0f;

        // Set player collider size (centered on position, body only — excludes weapon)
        auto& playerCollider = m_Registry.GetComponent<Collider>(m_PlayerEntity);
        playerCollider.Width = 0.6f;
        playerCollider.Height = 1.8f;
        playerCollider.Depth = 0.6f;
        playerCollider.IsStatic = false;

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

        // Convert to MapGrid
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

        // Add static colliders to wall prefabs
        if (IsWallPrefab(type))
        {
            SpawnWallColliders(type, position, rotY);
        }

        // Add trigger collider and Door component to doors
        if (type == engine::PrefabType::Door)
        {
            Collider doorTrigger(2.5f, 3.0f, 2.5f);
            doorTrigger.IsTrigger = true;
            doorTrigger.IsStatic = false;
            m_Registry.AddComponent(e, doorTrigger);

            Door door;
            door.BaseRotationY = glm::radians(rotY);
            m_Registry.AddComponent(e, door);
        }

        return e;
    }

    void GameplayScene::SpawnColliderEntity(const glm::vec3& position, float width, float height, float depth)
    {
        engine::Entity e = m_Registry.CreateEntity();
        m_Registry.AddComponent(e, Transform(position.x, position.y, position.z));

        Collider c(width, height, depth);
        c.IsStatic = true;
        m_Registry.AddComponent(e, c);
    }

    void GameplayScene::SpawnWallColliders(engine::PrefabType type, const glm::vec3& position, float rotY)
    {
        // Wall arm dimensions: spans one tile length (4 units), thin (0.5 units)
        constexpr float armLength = 4.0f;
        constexpr float armThick  = 0.5f;
        constexpr float wallH     = 3.0f;
        // Half-arm offset from center to place each arm
        constexpr float halfArm   = armLength * 0.5f;

        // Normalize rotation to determine which arms exist
        float normRot = std::fmod(rotY, 360.0f);
        if (normRot < 0.0f) normRot += 360.0f;

        if (type == engine::PrefabType::Wall)
        {
            // Straight wall: one arm centered at position
            // rotY=0 means E-W (along X), rotY=90 means N-S (along Z)
            bool vertical = (normRot > 45.0f && normRot < 135.0f) ||
                            (normRot > 225.0f && normRot < 315.0f);

            if (vertical)
                SpawnColliderEntity(position, armThick, wallH, armLength);
            else
                SpawnColliderEntity(position, armLength, wallH, armThick);
        }
        else if (type == engine::PrefabType::WallCorner || type == engine::PrefabType::WallCornerSmall)
        {
            // Corner: L-shape = 2 arms. The corner mesh sits at the junction.
            // Determine which two directions the arms extend based on rotation.
            // rotY=0: S+W corner arms extend along -X and +Z
            // rotY=90: E+S arms extend along +X and +Z
            // rotY=180: N+E arms extend along +X and -Z
            // rotY=-90/270: W+N arms extend along -X and -Z
            float dx1 = 0, dz1 = 0, dx2 = 0, dz2 = 0;

            int rot = static_cast<int>(normRot + 0.5f) % 360;
            if (rot < 45 || rot >= 315) { // 0: S+W
                dx1 = -halfArm; dz1 = 0;   // arm along -X
                dx2 = 0;        dz2 = halfArm; // arm along +Z
            } else if (rot < 135) { // 90: E+S
                dx1 = halfArm;  dz1 = 0;   // arm along +X
                dx2 = 0;        dz2 = halfArm; // arm along +Z
            } else if (rot < 225) { // 180: N+E
                dx1 = halfArm;  dz1 = 0;   // arm along +X
                dx2 = 0;        dz2 = -halfArm; // arm along -Z
            } else { // 270: W+N
                dx1 = -halfArm; dz1 = 0;   // arm along -X
                dx2 = 0;        dz2 = -halfArm; // arm along -Z
            }

            // Horizontal arm (along X)
            SpawnColliderEntity(position + glm::vec3(dx1, 0, 0), armLength, wallH, armThick);
            // Vertical arm (along Z)
            SpawnColliderEntity(position + glm::vec3(0, 0, dz2), armThick, wallH, armLength);
        }
        else if (type == engine::PrefabType::WallTsplit)
        {
            // T-split: 3 arms. The missing direction determines shape.
            // rotY=0: missing N → arms go E, W, S
            // rotY=90: missing W → arms go N, S, E
            // rotY=180: missing S → arms go E, W, N
            // rotY=-90/270: missing E → arms go N, S, W
            int rot = static_cast<int>(normRot + 0.5f) % 360;

            bool hasN = true, hasS = true, hasE = true, hasW = true;
            if (rot < 45 || rot >= 315)      hasN = false; // 0
            else if (rot < 135)              hasW = false; // 90
            else if (rot < 225)              hasS = false; // 180
            else                             hasE = false; // 270

            // Horizontal bar (E-W) if both E and W exist
            if (hasE && hasW)
                SpawnColliderEntity(position, armLength, wallH, armThick);

            // Vertical bar (N-S) if both N and S exist
            if (hasN && hasS)
                SpawnColliderEntity(position, armThick, wallH, armLength);

            // Individual arms for the T-shape
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
            // Crossing: 4-way junction = full cross
            SpawnColliderEntity(position, armLength, wallH, armThick); // E-W bar
            SpawnColliderEntity(position, armThick, wallH, armLength); // N-S bar
        }
    }

    void GameplayScene::OnUpdate(engine::Application& application, const engine::Timestep timestep)
    {
        const float dt = timestep.GetSeconds();
        const engine::Input& input = application.GetInput();

        // Debug systems
        m_DebugToggle.Update(input);
        m_FPSCounter.Update(timestep);

        // Update window title with FPS when debug mode is on
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
        m_CollisionSystem.Update(m_Registry);
        AnimationSystem::Update(m_Registry, dt);
        BoneAttachmentSystem::Update(m_Registry);
        UpdateAttackHitbox();
        UpdateDoors(dt);

        UpdateCamera(input);
    }

    void GameplayScene::UpdateAttackHitbox()
    {
        if (!m_Registry.IsAlive(m_PlayerEntity) ||
            !m_Registry.HasComponent<AnimationState>(m_PlayerEntity))
            return;

        const auto& anim = m_Registry.GetComponent<AnimationState>(m_PlayerEntity);
        const auto& playerTransform = m_Registry.GetComponent<Transform>(m_PlayerEntity);

        if (anim.IsAttacking)
        {
            // Spawn hitbox if it doesn't exist
            if (m_AttackHitbox.GetId() == 0 || !m_Registry.IsAlive(m_AttackHitbox))
            {
                m_AttackHitbox = m_Registry.CreateEntity();

                Collider hitboxCollider(1.2f, 1.5f, 1.2f);
                hitboxCollider.IsTrigger = true;
                hitboxCollider.IsStatic = false;
                m_Registry.AddComponent(m_AttackHitbox, Transform());
                m_Registry.AddComponent(m_AttackHitbox, hitboxCollider);
            }

            // Position hitbox in front of the player based on facing direction
            float offsetDist = 1.0f;
            float hitX = playerTransform.X + std::sin(playerTransform.RotationY) * offsetDist;
            float hitZ = playerTransform.Z + std::cos(playerTransform.RotationY) * offsetDist;

            auto& hitTransform = m_Registry.GetComponent<Transform>(m_AttackHitbox);
            hitTransform.X = hitX;
            hitTransform.Y = playerTransform.Y + 0.5f;
            hitTransform.Z = hitZ;
        }
        else
        {
            // Destroy hitbox when not attacking
            if (m_AttackHitbox.GetId() != 0 && m_Registry.IsAlive(m_AttackHitbox))
            {
                m_Registry.DestroyEntity(m_AttackHitbox);
                m_AttackHitbox = engine::Entity(0);
            }
        }
    }

    void GameplayScene::UpdateDoors(float dt)
    {
        if (!m_Registry.IsAlive(m_PlayerEntity))
            return;

        const auto& playerT = m_Registry.GetComponent<Transform>(m_PlayerEntity);

        for (const engine::Entity entity : m_Registry.GetActiveEntities())
        {
            if (!m_Registry.HasComponent<Door>(entity) ||
                !m_Registry.HasComponent<Transform>(entity))
                continue;

            auto& door = m_Registry.GetComponent<Door>(entity);
            auto& doorT = m_Registry.GetComponent<Transform>(entity);

            // Check if player is within trigger distance
            float dx = playerT.X - doorT.X;
            float dz = playerT.Z - doorT.Z;
            float distSq = dx * dx + dz * dz;
            bool playerNear = distSq < 2.5f * 2.5f;

            // Determine swing direction based on which side player approaches from
            if (playerNear && door.SwingDirection == 0)
            {
                // Door's local forward is determined by its base rotation
                // The door faces along its local Z axis
                float doorForwardX = std::sin(door.BaseRotationY);
                float doorForwardZ = std::cos(door.BaseRotationY);

                // Dot product of player-to-door direction with door forward
                float dot = dx * doorForwardX + dz * doorForwardZ;
                door.SwingDirection = (dot > 0.0f) ? 1 : -1;
            }

            // Reset swing direction when door fully closes
            if (!playerNear && std::abs(door.CurrentAngle) < 0.1f)
            {
                door.SwingDirection = 0;
            }

            // Animate toward target
            float goalAngle = playerNear ? 90.0f * door.SwingDirection : 0.0f;
            float step = door.SwingSpeed * dt;

            if (door.CurrentAngle < goalAngle)
            {
                door.CurrentAngle += step;
                if (door.CurrentAngle > goalAngle)
                    door.CurrentAngle = goalAngle;
            }
            else if (door.CurrentAngle > goalAngle)
            {
                door.CurrentAngle -= step;
                if (door.CurrentAngle < goalAngle)
                    door.CurrentAngle = goalAngle;
            }

            // Apply rotation: base rotation + swing offset
            doorT.RotationY = door.BaseRotationY + glm::radians(door.CurrentAngle);
        }
    }

    void GameplayScene::UpdateCamera(const engine::Input& input)
    {
        if (!m_Registry.IsAlive(m_PlayerEntity))
            return;

        // Scroll wheel zoom
        float scroll = input.GetMouseWheelY();
        if (scroll != 0.0f)
        {
            m_CameraZoom -= scroll * kZoomSpeed;
            if (m_CameraZoom < kZoomMin) m_CameraZoom = kZoomMin;
            if (m_CameraZoom > kZoomMax) m_CameraZoom = kZoomMax;
        }

        const auto& transform = m_Registry.GetComponent<Transform>(m_PlayerEntity);
        float zOffset = m_CameraZoom * 0.4f; // proportional Z offset
        m_Camera.SetPosition(transform.X, m_CameraZoom, transform.Z + zOffset);
        m_Camera.SetTarget(transform.X, 0.0f, transform.Z);
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

            // Upload bone matrices (use cached locations)
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
                // Reset to identity only after rendering an animated entity
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
    }
}

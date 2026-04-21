// Created by Adam Van Woerden
/// @file GameplayScene.h
/// @brief Core gameplay scene -- dungeon rendering, ECS systems, player control.
///
/// Owns the ECS Registry, Camera, asset managers, and all gameplay systems.
/// On creation it loads the player model, animations, and dungeon prefabs,
/// then spawns the dungeon and player entity. ESC pushes PauseMenuScene.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H

#include "engine/scene/Scene.h"
#include "engine/scene/PrefabManager.h"
#include "engine/rendering/Camera.h"
#include "engine/resources/AssetManager.h"
#include "engine/resources/MeshManager.h"
#include "engine/resources/AnimationLoader.h"
#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

#include "game/systems/FPSCounter.h"
#include "game/systems/DebugToggle.h"
#include "game/systems/DebugColliderRenderer.h"
#include "game/systems/CollisionSystem.h"
#include "game/systems/AttackHitboxSystem.h"
#include "game/systems/CameraFollowSystem.h"
#include "game/systems/DungeonSpawnSystem.h"
#include "game/systems/EnemyAISystem.h"
#include "game/systems/HitTimerSystem.h"
#include "game/systems/DamageSystem.h"
#include "game/systems/LifetimeSystem.h"
#include "game/systems/SwitchTriggerSystem.h"
#include "game/systems/UISystem.h"
#include "engine/rendering/TextRenderer.h"

#include <memory>

#include "engine/resources/AudioEventQueue.h"

namespace engine
{
    class Mesh;
    class Shader;
    class Texture;
}

namespace game
{

    class GameplayScene final : public engine::Scene
    {
    public:
        GameplayScene();

        /// Initializes camera, shader, ECS registry, loads content, spawns dungeon.
        bool OnCreate(engine::Application& application) override;
        /// Releases all ECS entities, meshes, textures, and animation data.
        void OnDestroy() override;
        /// Runs all ECS systems (input, movement, collision, AI, combat, etc.).
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        /// Renders all entities with their meshes/textures, debug overlays, and HUD.
        void OnRender(engine::Application& application) override;

        engine::AudioEventQueue& getAudioEventQueue() {
            return audioEventQueue;
        }

    private:
        /// Loads the player mesh, skeleton, and all animation clip files.
        void LoadContent();
        /// Registers dungeon tile prefabs (walls, floors, doors, etc.).
        void RegisterPrefabs();
        /// Spawns the player entity with equipment and generates the dungeon.
        void InitializeScene();
        /// Searches loaded animation clips by name; returns -1 if not found.
        int FindClipIndex(const std::string& name) const;

        engine::Camera m_Camera;
        engine::AssetManager m_AssetManager;
        engine::MeshManager m_MeshManager;
        engine::AnimationLoader m_AnimationLoader;
        engine::Registry m_Registry;
        SDL_Renderer* m_SDLRenderer = nullptr;

        std::shared_ptr<engine::Shader> m_Shader;

        engine::Entity m_PlayerEntity;

        std::shared_ptr<std::vector<engine::AnimationClip>> m_PlayerClips;
        std::shared_ptr<engine::Skeleton> m_PlayerSkeleton;

        // Systems
        FPSCounter m_FPSCounter;
        DebugToggle m_DebugToggle;
        DebugColliderRenderer m_DebugColliderRenderer;
        std::unique_ptr<DungeonSpawnSystem> m_DungeonSpawnSystem;
        UISystem m_UISystem;
        engine::TextRenderer m_DebugTextRenderer;
        engine::AudioEventQueue audioEventQueue;
    };
}

#endif

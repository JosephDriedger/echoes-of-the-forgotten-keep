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

#include <memory>

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

        bool OnCreate(engine::Application& application) override;
        void OnDestroy() override;
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        void OnRender(engine::Application& application) override;

        static void RegisterPrefab();
        engine::Entity SpawnPrefab(
            engine::PrefabType type,
            const glm::vec3& position,
            float rotY);

    private:
        void LoadContent();
        void InitializeScene();
        void UpdateCamera(const engine::Input& input);
        void UpdateAttackHitbox();
        void UpdateDoors(float dt);

        int FindClipIndex(const std::string& name) const;

        static bool IsWallPrefab(engine::PrefabType type);
        void SpawnColliderEntity(const glm::vec3& position, float width, float height, float depth);
        void SpawnWallColliders(engine::PrefabType type, const glm::vec3& position, float rotY);

        engine::Camera m_Camera;
        engine::AssetManager m_AssetManager;
        engine::MeshManager m_MeshManager;
        engine::AnimationLoader m_AnimationLoader;
        engine::Registry m_Registry;

        std::shared_ptr<engine::Shader> m_Shader;

        engine::Entity m_PlayerEntity;

        std::shared_ptr<std::vector<engine::AnimationClip>> m_PlayerClips;
        std::shared_ptr<engine::Skeleton> m_PlayerSkeleton;

        // Attack hitbox
        engine::Entity m_AttackHitbox{0};

        // Debug systems
        FPSCounter m_FPSCounter;
        DebugToggle m_DebugToggle;
        DebugColliderRenderer m_DebugColliderRenderer;
        CollisionSystem m_CollisionSystem;

        // Camera zoom
        float m_CameraZoom = 20.0f;
        static constexpr float kZoomMin = 8.0f;
        static constexpr float kZoomMax = 60.0f;
        static constexpr float kZoomSpeed = 0.8f;
    };
}

#endif

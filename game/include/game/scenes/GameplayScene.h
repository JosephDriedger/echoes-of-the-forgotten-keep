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
#include "game/systems/CombatSystem.h"
#include "game/systems/DamageSystem.h"
#include "game/systems/DoorPuzzleSystem.h"

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

    private:
        void LoadContent();
        void RegisterPrefabs();
        void InitializeScene();
        int FindClipIndex(const std::string& name) const;

        engine::Camera m_Camera;
        engine::AssetManager m_AssetManager;
        engine::MeshManager m_MeshManager;
        engine::AnimationLoader m_AnimationLoader;
        engine::Registry m_Registry;

        std::shared_ptr<engine::Shader> m_Shader;

        engine::Entity m_PlayerEntity;

        std::shared_ptr<std::vector<engine::AnimationClip>> m_PlayerClips;
        std::shared_ptr<engine::Skeleton> m_PlayerSkeleton;

        // Systems
        FPSCounter m_FPSCounter;
        DebugToggle m_DebugToggle;
        DebugColliderRenderer m_DebugColliderRenderer;
        CollisionSystem m_CollisionSystem;
        AttackHitboxSystem m_AttackHitboxSystem;
        CameraFollowSystem m_CameraFollowSystem;
        std::unique_ptr<DungeonSpawnSystem> m_DungeonSpawnSystem;
        EnemyAISystem m_EnemyAISystem;
        CombatSystem m_CombatSystem;
        DamageSystem m_DamageSystem;
    };
}

#endif

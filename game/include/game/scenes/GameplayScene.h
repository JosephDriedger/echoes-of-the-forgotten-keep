#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H

#include "engine/scene/Scene.h"
#include "engine/scene/Prefab.h"
#include "engine/rendering/Camera.h"
#include "engine/rendering/RenderSystem.h"
#include "engine/resources/AssetManager.h"
#include "engine/resources/MeshManager.h"
#include "engine/resources/AnimationLoader.h"
#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

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
        glm::vec3 position,
        float rotY);
    private:
        void LoadContent();
        void InitializeScene();
        void UpdateCamera();



        engine::Entity SpawnDungeonPiece(
            const std::string& meshPath,
            const std::string& texturePath,
            float x, float y, float z,
            float rotationYDegrees = 0.0f);

        int FindClipIndex(const std::string& name) const;

        engine::Camera m_Camera;
        engine::RenderSystem m_RenderSystem;
        engine::AssetManager m_AssetManager;
        engine::MeshManager m_MeshManager;
        engine::AnimationLoader m_AnimationLoader;
        engine::Registry m_Registry;

        std::shared_ptr<engine::Shader> m_Shader;

        engine::Entity m_PlayerEntity;

        std::shared_ptr<std::vector<engine::AnimationClip>> m_PlayerClips;
        std::shared_ptr<engine::Skeleton> m_PlayerSkeleton;
    };
}

#endif

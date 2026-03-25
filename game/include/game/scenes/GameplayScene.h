//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H

#include "engine/scene/Scene.h"
#include "engine/rendering/Camera.h"
#include "engine/rendering/RenderSystem.h"
#include "engine/resources/AssetManager.h"
#include "engine/resources/MeshLoader.h"

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
        engine::Camera m_Camera;
        engine::RenderSystem m_RenderSystem;
        engine::AssetManager m_AssetManager;
        engine::MeshLoader m_MeshLoader;

        std::shared_ptr<engine::Mesh> m_Mesh;
        std::shared_ptr<engine::Shader> m_Shader;
        std::shared_ptr<engine::Texture> m_Texture;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H
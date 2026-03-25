//
// Created by Joseph Driedger on 3/8/2026.
//

#include "game/scenes/GameplayScene.h"

#include "engine/core/Application.h"
#include "engine/rendering/Renderer.h"

#include <SDL3/SDL_keycode.h>

namespace game
{
    GameplayScene::GameplayScene()
        : engine::Scene("GameplayScene"),
          m_Camera(),
          m_RenderSystem(),
          m_AssetManager(),
          m_MeshLoader(),
          m_Mesh(),
          m_Shader(),
          m_Texture()
    {
    }

    bool GameplayScene::OnCreate(engine::Application& application)
    {
        const engine::ApplicationSpecification& specification = application.GetSpecification();

        m_Camera.SetPosition(0.0f, 0.0f, 3.0f);
        m_Camera.SetTarget(0.0f, 0.0f, 0.0f);
        m_Camera.SetPerspective(
            45.0f,
            static_cast<float>(specification.Width) / static_cast<float>(specification.Height),
            0.1f,
            100.0f);

        m_Mesh = m_MeshLoader.LoadQuad();
        m_Shader = m_AssetManager.GetShaderManager().Load(
            "default",
            "asset/shaders/vertex.glsl",
            "asset/shaders/fragment.glsl");
        m_Texture = m_AssetManager.GetTextureManager().Load("asset/textures/debug.png");

        application.GetRenderer().SetActiveCamera(&m_Camera);

        return m_Mesh != nullptr && m_Shader != nullptr;
    }

    void GameplayScene::OnDestroy()
    {
        m_Texture.reset();
        m_Shader.reset();
        m_Mesh.reset();
        m_AssetManager.Clear();
    }

    void GameplayScene::OnUpdate(engine::Application& application, const engine::Timestep timestep)
    {
        (void)timestep;

        if (application.GetInput().IsKeyPressed(SDLK_ESCAPE))
        {
            application.RequestQuit();
        }
    }

    void GameplayScene::OnRender(engine::Application& application)
    {
        (void)application;

        if (!m_Mesh || !m_Shader)
        {
            return;
        }

        m_Shader->Bind();
        m_Shader->SetMat4("view", m_Camera.GetViewMatrix());
        m_Shader->SetMat4("projection", m_Camera.GetProjectionMatrix());

        static const float identity[16] =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        m_Shader->SetMat4("model", identity);

        if (m_Texture)
        {
            m_Shader->SetInt("texture_diffuse1", 0);
            m_RenderSystem.Draw(*m_Mesh, *m_Shader, *m_Texture);
            return;
        }

        m_RenderSystem.Draw(*m_Mesh, *m_Shader);
    }
}
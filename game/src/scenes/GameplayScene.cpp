//
// Created by Joseph Driedger on 3/8/2026.
//

#include "game/scenes/GameplayScene.h"

#include "engine/core/Application.h"

#include <SDL3/SDL_keycode.h>
#include <glad/gl.h>

namespace game
{
    GameplayScene::GameplayScene()
        : engine::Scene("GameplayScene")
    {
    }

    bool GameplayScene::OnCreate(engine::Application& application)
    {
        (void)application;

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);

        return true;
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
        const engine::ApplicationSpecification& specification = application.GetSpecification();

        glViewport(0, 0, specification.Width, specification.Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
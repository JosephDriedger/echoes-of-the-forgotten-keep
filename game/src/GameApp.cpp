//
// Created by Joseph Driedger on 3/8/2026.
//

#include "game/GameApp.h"

#include <SDL3/SDL_keycode.h>
#include <glad/gl.h>

#include <iostream>

namespace game
{
    GameApp::GameApp()
        : m_State(GameState::MainMenu)
    {
        m_Specification.Title = "Echoes of the Forgotten Keep";
        m_Specification.Width = 1280;
        m_Specification.Height = 720;
        m_Specification.TargetFrameTimeMs = 1000.0 / 60.0;
    }

    int GameApp::Run()
    {
        engine::Application application(m_Specification);
        return application.Run(*this);
    }

    bool GameApp::OnInitialize(engine::Application& application)
    {
        (void)application;

        std::cout << "[EFK] Entering main loop.\n";
        m_State = GameState::Playing;

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);

        return true;
    }

    void GameApp::OnShutdown(engine::Application& application)
    {
        (void)application;
        m_State = GameState::Quitting;
        std::cout << "[EFK] Shutdown complete.\n";
    }

    void GameApp::OnUpdate(engine::Application& application, engine::Timestep timestep)
    {
        if (application.GetInput().IsKeyPressed(SDLK_ESCAPE))
        {
            application.RequestQuit();
            return;
        }

        switch (m_State)
        {
            case GameState::MainMenu:
                break;

            case GameState::Playing:
            {
                const float deltaTime = timestep.GetSecondsFloat();
                (void)deltaTime;
                break;
            }

            case GameState::Paused:
                break;

            case GameState::GameOver:
                break;

            case GameState::Quitting:
                application.RequestQuit();
                break;

            default:
                break;
        }
    }

    void GameApp::OnRender(engine::Application& application)
    {
        const engine::ApplicationSpecification& specification = application.GetSpecification();

        glViewport(0, 0, specification.Width, specification.Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
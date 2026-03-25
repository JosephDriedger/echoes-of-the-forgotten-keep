//
// Created by Joseph Driedger on 3/8/2026.
//

#include "game/GameApp.h"

#include "game/scenes/GameplayScene.h"

#include <iostream>

namespace game
{
    GameApp::GameApp()
        : m_SceneManager()
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
        std::cout << "[EFK] Entering main loop.\n";

        if (!m_SceneManager.Initialize(application))
        {
            return false;
        }

        m_SceneManager.ChangeScene<GameplayScene>();
        return true;
    }

    void GameApp::OnShutdown(engine::Application& application)
    {
        (void)application;

        m_SceneManager.Shutdown();
        std::cout << "[EFK] Shutdown complete.\n";
    }

    void GameApp::OnEvent(engine::Application& application, const SDL_Event& event)
    {
        (void)application;
        (void)event;
    }

    void GameApp::OnUpdate(engine::Application& application, const engine::Timestep timestep)
    {
        (void)application;
        m_SceneManager.Update(timestep);
    }

    void GameApp::OnRender(engine::Application& application)
    {
        (void)application;
        m_SceneManager.Render();
    }
}
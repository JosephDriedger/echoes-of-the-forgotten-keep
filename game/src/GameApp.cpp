#include "game/GameApp.h"

#include "game/scenes/MainMenuScene.h"
#include "game/scenes/GameplayScene.h"
#include "game/scenes/SettingsScene.h"

#include <iostream>

namespace game
{
    GameApp::GameApp()
        : m_Specification(),
          m_SceneManager()
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
        std::cout << "[GameApp] OnInitialize started.\n";

        if (!m_SceneManager.Initialize(application))
        {
            std::cerr << "[GameApp] SceneManager initialization failed.\n";
            return false;
        }

        std::cout << "[GameApp] SceneManager initialized.\n";
        std::cout << "[GameApp] Changing to MainMenuScene.\n";

        if (!m_SceneManager.ChangeScene<MainMenuScene>())
        {
            std::cerr << "[GameApp] Failed to activate MainMenuScene.\n";
            return false;
        }

        std::cout << "[GameApp] MainMenuScene activated.\n";
        std::cout << "[EFK] Entering main loop.\n";
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

        // Handle deferred scene change requests
        const std::string& requested = application.GetRequestedScene();
        if (!requested.empty())
        {
            if (requested == "GameplayScene")
            {
                m_SceneManager.ChangeScene<GameplayScene>();
            }
            else if (requested == "MainMenuScene")
            {
                m_SceneManager.ChangeScene<MainMenuScene>();
            }
            else if (requested == "SettingsScene")
            {
                m_SceneManager.ChangeScene<SettingsScene>();
            }

            application.ClearSceneChangeRequest();
        }
    }

    void GameApp::OnRender(engine::Application& application)
    {
        (void)application;
        m_SceneManager.Render();
    }
}

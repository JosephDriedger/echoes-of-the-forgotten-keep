//
// Created by Joseph Driedger on 3/8/2026.
//

#include "game/GameApp.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>
#include <glad/gl.h>
#include <cmath>
#include <iostream>

namespace game
{
    GameApp::GameApp()
        : m_State(GameState::MainMenu),
          m_TargetFrameTimeMs(1000.0 / 60.0)
    {
    }

    int GameApp::Run()
    {
        engine::Application application;

        if (!application.Initialize())
        {
            std::cerr << "[EFK] Failed to initialize application.\n";
            return -1;
        }

        std::cout << "[EFK] Entering main loop.\n";

        int exitCode = 0;
        bool isRunning = true;
        m_State = GameState::Playing;

        while (isRunning)
        {
            const Uint64 frameStart = SDL_GetTicks();

            const engine::Timestep timestep = application.Tick();

            isRunning = application.PollEvents();

            if (isRunning && application.GetInput().IsKeyPressed(SDLK_ESCAPE))
            {
                isRunning = false;
            }

            Update(timestep);
            Render(application);

            const Uint64 frameEnd = SDL_GetTicks();
            const double frameTimeMs = static_cast<double>(frameEnd - frameStart);

            if (frameTimeMs < m_TargetFrameTimeMs)
            {
                const double remainingMs = m_TargetFrameTimeMs - frameTimeMs;
                SDL_Delay(static_cast<Uint32>(std::ceil(remainingMs)));
            }
        }

        m_State = GameState::Quitting;
        application.Shutdown();

        std::cout << "[EFK] Shutdown complete.\n";

        return exitCode;
    }

    void GameApp::Update(engine::Timestep timestep)
    {
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
                break;

            default:
                break;
        }
    }

    void GameApp::Render(engine::Application& application) const
    {
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        application.Present();
    }
}
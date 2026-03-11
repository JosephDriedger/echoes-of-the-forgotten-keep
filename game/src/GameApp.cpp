//
// Created by Joseph Driedger on 3/8/2026.
//

#include "game/GameApp.h"

#include "engine/core/Application.h"

#include <SDL3/SDL.h>
#include <iostream>

namespace game
{
    GameApp::GameApp()
        : m_State(GameState::MainMenu),
          m_TargetFrameTimeMs(16)
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

            ProcessEvents(isRunning);
            Update();
            Render();

            const Uint64 frameEnd = SDL_GetTicks();
            const Uint64 frameTime = frameEnd - frameStart;

            if (frameTime < m_TargetFrameTimeMs)
            {
                SDL_Delay(static_cast<Uint32>(m_TargetFrameTimeMs - frameTime));
            }
        }

        m_State = GameState::Quitting;
        application.Shutdown();

        std::cout << "[EFK] Shutdown complete.\n";

        return exitCode;
    }

    void GameApp::ProcessEvents(bool& isRunning)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                isRunning = false;
                return;
            }

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    isRunning = false;
                    return;
                }
            }
        }
    }

    void GameApp::Update()
    {
        switch (m_State)
        {
            case GameState::MainMenu:
            case GameState::Playing:
            case GameState::Paused:
            case GameState::GameOver:
            case GameState::Quitting:
            default:
                break;
        }
    }

    void GameApp::Render()
    {
    }
}

//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/core/Application.h"
#include "engine/platform/Window.h"

#include <SDL3/SDL.h>
#include <iostream>

namespace engine
{

    Application::Application()
    {
    }

    Application::~Application()
    {
        Shutdown();
    }

    bool Application::Initialize()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        m_Window = std::make_unique<Window>();

        if (!m_Window->Create("Echoes of the Forgotten Keep", 1280, 720))
        {
            return false;
        }

        return true;
    }

    void Application::Shutdown()
    {
        m_Window.reset();
        SDL_Quit();
    }

}
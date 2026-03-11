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
        : m_IsInitialized(false)
    {
    }

    Application::~Application()
    {
        Shutdown();
    }

    bool Application::Initialize()
    {
        if (m_IsInitialized)
        {
            return true;
        }

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        m_Window = std::make_unique<Window>();

        if (!m_Window->Create("Echoes of the Forgotten Keep", 1280, 720))
        {
            m_Window.reset();
            m_Input.Reset();
            SDL_Quit();
            return false;
        }

        m_Input.Reset();
        m_Time.Reset();
        m_IsInitialized = true;

        return true;
    }

    void Application::Shutdown()
    {
        if (!m_IsInitialized)
        {
            return;
        }

        m_Input.Reset();
        m_Window.reset();
        SDL_Quit();
        m_IsInitialized = false;
    }

    bool Application::PollEvents()
    {
        if (!m_IsInitialized)
        {
            return false;
        }

        m_Input.BeginFrame();

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            m_Input.ProcessEvent(event);
        }

        return !m_Input.IsQuitRequested();
    }

    Timestep Application::Tick()
    {
        if (!m_IsInitialized)
        {
            return Timestep(0.0);
        }

        return m_Time.Tick();
    }

    void Application::Present() const
    {
        if (!m_IsInitialized || !m_Window)
        {
            return;
        }

        m_Window->SwapBuffers();
    }

    const Input& Application::GetInput() const
    {
        return m_Input;
    }

    Input& Application::GetInput()
    {
        return m_Input;
    }
}
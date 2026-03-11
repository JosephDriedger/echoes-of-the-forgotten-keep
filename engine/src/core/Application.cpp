//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/core/Application.h"
#include "engine/platform/Window.h"
#include "engine/rendering/Renderer.h"

#include <SDL3/SDL.h>

#include <cmath>
#include <iostream>

namespace engine
{
    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification),
          m_IsInitialized(false),
          m_IsRunning(false)
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

        if (!m_Window->Create(
                m_Specification.Title,
                m_Specification.Width,
                m_Specification.Height))
        {
            m_Window.reset();
            m_Input.Reset();
            SDL_Quit();
            return false;
        }

        m_Renderer = std::make_unique<Renderer>();

        if (!m_Renderer->Initialize(*m_Window))
        {
            m_Renderer.reset();
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

        m_IsRunning = false;
        m_Input.Reset();

        if (m_Renderer)
        {
            m_Renderer->Shutdown();
            m_Renderer.reset();
        }

        m_Window.reset();
        SDL_Quit();
        m_IsInitialized = false;
    }

    int Application::Run(IApplicationListener& listener)
    {
        if (!Initialize())
        {
            return -1;
        }

        if (!listener.OnInitialize(*this))
        {
            std::cerr << "[Engine] Listener initialization failed.\n";
            Shutdown();
            return -1;
        }

        m_IsRunning = true;

        while (m_IsRunning)
        {
            const Uint64 frameStart = SDL_GetTicks();

            if (!PumpEvents(listener))
            {
                break;
            }

            const Timestep timestep = Tick();

            listener.OnUpdate(*this, timestep);

            if (!m_IsRunning)
            {
                break;
            }

            RenderFrame(listener);

            const Uint64 frameEnd = SDL_GetTicks();
            const double frameTimeMs = static_cast<double>(frameEnd - frameStart);

            if (m_Specification.TargetFrameTimeMs > 0.0 &&
                frameTimeMs < m_Specification.TargetFrameTimeMs)
            {
                const double remainingMs = m_Specification.TargetFrameTimeMs - frameTimeMs;
                SDL_Delay(static_cast<Uint32>(std::ceil(remainingMs)));
            }
        }

        listener.OnShutdown(*this);
        Shutdown();

        return 0;
    }

    void Application::RequestQuit()
    {
        m_IsRunning = false;
    }

    bool Application::IsRunning() const
    {
        return m_IsRunning;
    }

    bool Application::PumpEvents(IApplicationListener& listener)
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
            listener.OnEvent(*this, event);
        }

        if (m_Input.IsQuitRequested())
        {
            RequestQuit();
        }

        return m_IsRunning;
    }

    void Application::RenderFrame(IApplicationListener& listener)
    {
        if (!m_IsInitialized || !m_Renderer || !m_Renderer->IsInitialized())
        {
            return;
        }

        m_Renderer->BeginFrame();
        listener.OnRender(*this);
        m_Renderer->EndFrame();
    }

    Timestep Application::Tick()
    {
        if (!m_IsInitialized)
        {
            return Timestep(0.0);
        }

        return m_Time.Tick();
    }

    const Input& Application::GetInput() const
    {
        return m_Input;
    }

    Input& Application::GetInput()
    {
        return m_Input;
    }

    const ApplicationSpecification& Application::GetSpecification() const
    {
        return m_Specification;
    }
}
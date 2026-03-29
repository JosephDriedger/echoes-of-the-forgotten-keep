//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/core/Application.h"

#include "engine/platform/Window.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <iostream>

namespace engine
{
    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification),
          m_Window(nullptr),
          m_Renderer(),
          m_Input(),
          m_IsRunning(false)
    {
    }

    Application::~Application()
    {
        Shutdown();
    }

    int Application::Run(IApplicationListener& listener)
    {
        std::cout << "[Application] Run started.\n";

        if (!Initialize())
        {
            std::cerr << "[Application] Initialize() failed.\n";
            return -1;
        }

        std::cout << "[Application] Initialize() succeeded.\n";

        if (!listener.OnInitialize(*this))
        {
            std::cerr << "[Application] Listener OnInitialize() failed.\n";
            Shutdown();
            return -1;
        }

        std::cout << "[Application] Listener OnInitialize() succeeded.\n";

        m_IsRunning = true;

        Uint64 previousCounter = SDL_GetPerformanceCounter();
        const double performanceFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

        while (m_IsRunning)
        {
            const Uint64 currentCounter = SDL_GetPerformanceCounter();
            const double deltaSeconds =
                static_cast<double>(currentCounter - previousCounter) / performanceFrequency;
            previousCounter = currentCounter;

            ProcessEvents(listener);

            const Timestep timestep(deltaSeconds);

            listener.OnUpdate(*this, timestep);

            m_Renderer.BeginFrame();
            listener.OnRender(*this);
            m_Renderer.EndFrame();
        }

        listener.OnShutdown(*this);
        Shutdown();

        std::cout << "[Application] Shutdown complete.\n";
        return 0;
    }

    void Application::RequestQuit()
    {
        m_IsRunning = false;
    }

    Renderer& Application::GetRenderer()
    {
        return m_Renderer;
    }

    Input& Application::GetInput()
    {
        return m_Input;
    }

    const ApplicationSpecification& Application::GetSpecification() const
    {
        return m_Specification;
    }

    bool Application::Initialize()
    {
        std::cout << "[Application] Initializing SDL.\n";

        SDL_SetMainReady();

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "[Application] SDL_Init failed: " << SDL_GetError() << '\n';
            return false;
        }

        std::cout << "[Application] SDL initialized.\n";

        m_Window = new Window();

        std::cout << "[Application] Creating window.\n";

        if (!m_Window->Create(
            m_Specification.Title.c_str(),
            m_Specification.Width,
            m_Specification.Height))
        {
            std::cerr << "[Application] Window creation failed.\n";
            delete m_Window;
            m_Window = nullptr;
            SDL_Quit();
            return false;
        }

        std::cout << "[Application] Window created.\n";
        std::cout << "[Application] Initializing renderer.\n";

        if (!m_Renderer.Initialize(*m_Window))
        {
            std::cerr << "[Application] Renderer initialization failed.\n";
            Shutdown();
            return false;
        }

        std::cout << "[Application] Renderer initialized.\n";
        return true;
    }

    void Application::Shutdown()
    {
        m_Renderer.Shutdown();

        if (m_Window)
        {
            m_Window->Destroy();
            delete m_Window;
            m_Window = nullptr;
        }

        SDL_Quit();
    }

    void Application::ProcessEvents(IApplicationListener& listener)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            m_Input.ProcessEvent(event);
            listener.OnEvent(*this, event);

            if (event.type == SDL_EVENT_QUIT)
            {
                RequestQuit();
            }
        }
    }
}
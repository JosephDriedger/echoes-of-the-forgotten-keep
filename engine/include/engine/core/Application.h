//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H

#include "engine/core/Time.h"
#include "engine/input/Input.h"

#include <SDL3/SDL.h>

#include <memory>

namespace engine
{
    class Window;

    struct ApplicationSpecification
    {
        const char* Title = "Engine Application";
        int Width = 1280;
        int Height = 720;
        double TargetFrameTimeMs = 1000.0 / 60.0;
    };

    class Application;

    class IApplicationListener
    {
    public:
        virtual ~IApplicationListener() = default;

        virtual bool OnInitialize(Application& application)
        {
            (void)application;
            return true;
        }

        virtual void OnShutdown(Application& application)
        {
            (void)application;
        }

        virtual void OnEvent(Application& application, const SDL_Event& event)
        {
            (void)application;
            (void)event;
        }

        virtual void OnUpdate(Application& application, Timestep timestep) = 0;
        virtual void OnRender(Application& application) = 0;
    };

    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& specification = ApplicationSpecification());
        ~Application();

        bool Initialize();
        void Shutdown();

        int Run(IApplicationListener& listener);

        void RequestQuit();
        bool IsRunning() const;

        Timestep Tick();
        void Present() const;

        const Input& GetInput() const;
        Input& GetInput();

        const ApplicationSpecification& GetSpecification() const;

    private:
        bool PumpEvents(IApplicationListener& listener);

    private:
        ApplicationSpecification m_Specification;
        std::unique_ptr<Window> m_Window;
        Input m_Input;
        Time m_Time;
        bool m_IsInitialized;
        bool m_IsRunning;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
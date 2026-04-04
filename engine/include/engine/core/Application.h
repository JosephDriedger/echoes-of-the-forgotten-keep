// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H

#include "engine/core/ApplicationSpecification.h"
#include "engine/core/Timestep.h"
#include "engine/input/Input.h"
#include "engine/rendering/Renderer.h"

#include <SDL3/SDL_events.h>
#include <memory>

namespace engine
{
    class Application;

    class IApplicationListener
    {
    public:
        virtual ~IApplicationListener() = default;

        virtual bool OnInitialize(Application& application) = 0;
        virtual void OnShutdown(Application& application) = 0;
        virtual void OnEvent(Application& application, const SDL_Event& event) = 0;
        virtual void OnUpdate(Application& application, Timestep timestep) = 0;
        virtual void OnRender(Application& application) = 0;
    };

    class Window;

    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& specification);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        int Run(IApplicationListener& listener);

        void RequestQuit();

        /// Request a deferred scene change. The name is read by GameApp
        /// after the current frame's Update() completes, so the active
        /// scene is not destroyed mid-update.
        void RequestSceneChange(const std::string& sceneName);
        const std::string& GetRequestedScene() const;
        void ClearSceneChangeRequest();

        Renderer& GetRenderer();
        Input& GetInput();
        Window* GetWindow();
        const ApplicationSpecification& GetSpecification() const;

    private:
        bool Initialize();
        void Shutdown();
        void ProcessEvents(IApplicationListener& listener);

    private:
        ApplicationSpecification m_Specification;
        std::unique_ptr<Window> m_Window;
        Renderer m_Renderer;
        Input m_Input;
        bool m_IsRunning;
        std::string m_RequestedScene;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
// Created by Joey Driedger

// Application -- the core engine class that owns the main loop, window,
// renderer, input system, and audio manager. Game-specific behaviour is
// injected via the IApplicationListener interface.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H

#include "engine/core/ApplicationSpecification.h"
#include "engine/core/Timestep.h"
#include "engine/input/Input.h"
#include "engine/rendering/Renderer.h"

#include <SDL3/SDL_events.h>
#include <memory>

#include "engine/resources/AudioManager.h"

namespace engine
{
    class Application;

    // Callback interface that the game layer implements to receive
    // lifecycle events (init, shutdown) and per-frame hooks (event,
    // update, render) from the engine's main loop.
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

        // Runs the main loop: initialises subsystems, then enters the
        // poll-update-render cycle until quit is requested. Returns 0
        // on success or -1 on initialisation failure.
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
        AudioManager& GetAudioManager() { return m_AudioManager; }

    private:
        AudioManager m_AudioManager;

        // Sets up SDL, creates the window and initialises the renderer.
        bool Initialize();
        void Shutdown();
        // Drains the SDL event queue, forwarding each event to Input and the listener.
        void ProcessEvents(IApplicationListener& listener);

    private:
        ApplicationSpecification m_Specification;
        std::unique_ptr<Window> m_Window;
        Renderer m_Renderer;
        Input m_Input;
        bool m_IsRunning;
        // Name of the scene to transition to; empty means no pending change.
        std::string m_RequestedScene;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
// Created by Joey Driedger
/// @file GameApp.h
/// @brief Top-level application listener that owns the scene stack and routes
///        deferred scene-change requests to concrete scene types.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H

#include "engine/core/Application.h"
#include "engine/core/ApplicationSpecification.h"
#include "engine/core/Timestep.h"
#include "engine/resources/AudioManager.h"
#include "engine/scene/SceneManager.h"

namespace game
{
    /// Entry point for the game. Sets up the window specification, initializes
    /// the SceneManager with MainMenuScene, and dispatches scene transitions
    /// each frame based on string requests from the active scene.
    class GameApp final : public engine::IApplicationListener
    {
    public:
        GameApp();

        /// Creates the Application and enters the main loop.
        int Run();

        /// Called once at startup. Initializes SceneManager, loads audio, starts music.
        bool OnInitialize(engine::Application& application) override;
        /// Called once at shutdown. Tears down the scene stack.
        void OnShutdown(engine::Application& application) override;
        void OnEvent(engine::Application& application, const SDL_Event& event) override;
        /// Called each frame. Updates the active scene, then processes any
        /// deferred scene-change request (push, pop, or replace).
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        /// Called each frame after OnUpdate. Delegates to SceneManager::Render.
        void OnRender(engine::Application& application) override;

    private:
        engine::ApplicationSpecification m_Specification;
        engine::SceneManager m_SceneManager;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
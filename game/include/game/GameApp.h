//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H

#include "engine/core/Application.h"
#include "engine/core/ApplicationSpecification.h"
#include "engine/core/Timestep.h"
#include "engine/scene/SceneManager.h"

namespace game
{
    class GameApp final : public engine::IApplicationListener
    {
    public:
        GameApp();

        int Run();

        bool OnInitialize(engine::Application& application) override;
        void OnShutdown(engine::Application& application) override;
        void OnEvent(engine::Application& application, const SDL_Event& event) override;
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        void OnRender(engine::Application& application) override;

    private:
        engine::ApplicationSpecification m_Specification;
        engine::SceneManager m_SceneManager;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
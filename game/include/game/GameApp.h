//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H

#include "engine/core/Application.h"
#include "engine/core/Timestep.h"
#include "game/GameState.h"

namespace game
{
    class GameApp
    {
    public:
        GameApp();

        int Run();

    private:
        void Update(engine::Timestep timestep);
        void Render(engine::Application& application) const;

    private:
        GameState m_State;
        double m_TargetFrameTimeMs;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
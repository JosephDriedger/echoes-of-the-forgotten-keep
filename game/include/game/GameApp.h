//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H

#pragma once

#include "game/GameState.h"

namespace game
{
    class GameApp
    {
    public:
        GameApp();

        int Run();

    private:
        void ProcessEvents(bool& isRunning);
        void Update();
        void Render();

    private:
        GameState m_State;
        unsigned int m_TargetFrameTimeMs;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEAPP_H
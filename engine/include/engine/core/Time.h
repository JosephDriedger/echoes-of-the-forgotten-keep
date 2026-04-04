// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TIME_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TIME_H

#include "engine/core/Timestep.h"

#include <SDL3/SDL.h>

namespace engine
{
    class Time
    {
    public:
        Time();

        void Reset();
        Timestep Tick();

        double GetTotalSeconds() const;

    private:
        Uint64 m_LastCounter;
        Uint64 m_Frequency;
        double m_TotalSeconds;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TIME_H
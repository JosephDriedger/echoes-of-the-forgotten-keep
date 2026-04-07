// Created by Joey Driedger

// Time -- high-resolution frame clock built on SDL performance counters.
// Call Reset() once before entering the loop, then Tick() each frame to
// obtain the clamped delta as a Timestep.

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

        // Initialises (or re-initialises) the clock, zeroing elapsed time.
        void Reset();

        // Advances the clock. Returns the clamped delta since the last Tick.
        Timestep Tick();

        double GetTotalSeconds() const;

    private:
        Uint64 m_LastCounter;   // SDL performance counter from previous Tick
        Uint64 m_Frequency;     // SDL performance counter frequency (ticks/sec)
        double m_TotalSeconds;  // Accumulated elapsed time in seconds
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TIME_H
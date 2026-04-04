// Created by Joey Driedger

#include "engine/core/Time.h"

namespace engine
{
    namespace
    {
        constexpr double MAX_FRAME_DELTA_SECONDS = 0.25;
    }

    Time::Time()
        : m_LastCounter(0),
          m_Frequency(0),
          m_TotalSeconds(0.0)
    {
    }

    void Time::Reset()
    {
        m_Frequency = SDL_GetPerformanceFrequency();

        if (m_Frequency == 0)
        {
            m_Frequency = 1;
        }

        m_LastCounter = SDL_GetPerformanceCounter();
        m_TotalSeconds = 0.0;
    }

    Timestep Time::Tick()
    {
        const Uint64 currentCounter = SDL_GetPerformanceCounter();

        if (m_LastCounter == 0 || m_Frequency == 0)
        {
            m_LastCounter = currentCounter;

            if (m_Frequency == 0)
            {
                m_Frequency = SDL_GetPerformanceFrequency();

                if (m_Frequency == 0)
                {
                    m_Frequency = 1;
                }
            }

            return Timestep(0.0);
        }

        double deltaSeconds =
            static_cast<double>(currentCounter - m_LastCounter) /
            static_cast<double>(m_Frequency);

        if (deltaSeconds < 0.0)
        {
            deltaSeconds = 0.0;
        }

        if (deltaSeconds > MAX_FRAME_DELTA_SECONDS)
        {
            deltaSeconds = MAX_FRAME_DELTA_SECONDS;
        }

        m_LastCounter = currentCounter;
        m_TotalSeconds += deltaSeconds;

        return Timestep(deltaSeconds);
    }

    double Time::GetTotalSeconds() const
    {
        return m_TotalSeconds;
    }
}
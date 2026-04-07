// Created by Joey Driedger

// Timestep -- lightweight value type representing a frame's elapsed
// duration. Stored internally in seconds (double) with convenience
// accessors for milliseconds and float conversions.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TIMESTEP_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TIMESTEP_H

namespace engine
{
    class Timestep
    {
    public:
        explicit Timestep(double seconds = 0.0)
            : m_Seconds(seconds)
        {
        }

        double GetSeconds() const
        {
            return m_Seconds;
        }

        float GetSecondsFloat() const
        {
            return static_cast<float>(m_Seconds);
        }

        double GetMilliseconds() const
        {
            return m_Seconds * 1000.0;
        }

        float GetMillisecondsFloat() const
        {
            return static_cast<float>(m_Seconds * 1000.0);
        }

    private:
        double m_Seconds;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TIMESTEP_H
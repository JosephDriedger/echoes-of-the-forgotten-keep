// Created by Elijah Fabon

#include "game/systems/FPSCounter.h"

#include <sstream>
#include <iomanip>

namespace game
{
    void FPSCounter::Update(const engine::Timestep& timestep)
    {
        m_FrameCount++;
        m_AccumulatedTime += timestep.GetSeconds();

        // Average over the accumulation window to reduce jitter in the readout
        if (m_AccumulatedTime >= UPDATE_INTERVAL)
        {
            m_FPS = static_cast<float>(m_FrameCount) / m_AccumulatedTime;
            m_FrameTimeMs = (m_AccumulatedTime / static_cast<float>(m_FrameCount)) * 1000.0f;

            std::ostringstream oss;
            oss << "FPS: " << static_cast<int>(m_FPS)
                << " (" << std::fixed << std::setprecision(1) << m_FrameTimeMs << " ms)";
            m_DisplayString = oss.str();

            m_FrameCount = 0;
            m_AccumulatedTime = 0.0f;
        }
    }
}

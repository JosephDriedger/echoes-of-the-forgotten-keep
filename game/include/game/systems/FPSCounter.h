// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_FPSCOUNTER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_FPSCOUNTER_H

#include "engine/core/Timestep.h"

#include <string>

namespace game
{
    class FPSCounter
    {
    public:
        void Update(const engine::Timestep& timestep);

        [[nodiscard]] float GetFPS() const { return m_FPS; }
        [[nodiscard]] float GetFrameTimeMs() const { return m_FrameTimeMs; }
        [[nodiscard]] const std::string& GetDisplayString() const { return m_DisplayString; }

    private:
        float m_FPS = 0.0f;
        float m_FrameTimeMs = 0.0f;
        float m_AccumulatedTime = 0.0f;
        int m_FrameCount = 0;
        std::string m_DisplayString = "FPS: --";

        static constexpr float UPDATE_INTERVAL = 0.5f;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_FPSCOUNTER_H

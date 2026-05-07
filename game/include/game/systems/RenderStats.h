//
// Created by adamd on 2026-05-06.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSTATS_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSTATS_H
#include <string>

#include "engine/core/Timestep.h"

namespace game
{
    class RenderStats
    {
    public:
        void BeginFrame();
        void OnEntityRendered();
        void OnEntityCulled();

        void Update(const engine::Timestep& timestep);

        const std::string& GetDisplayString() const { return m_DisplayString; }

    private:
        int m_Total = 0;
        int m_Rendered = 0;
        int m_Culled = 0;

        float m_AccumulatedTime = 0.0f;

        std::string m_DisplayString;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSTATS_H
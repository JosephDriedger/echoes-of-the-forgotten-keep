//
// Created by adamd on 2026-05-06.
//

#include "game/systems/RenderStats.h"
#include <sstream>
#include <iomanip>

namespace game
{
    void RenderStats::BeginFrame()
    {
        m_Total = 0;
        m_Rendered = 0;
        m_Culled = 0;
    }

    void RenderStats::OnEntityRendered()
    {
        m_Total++;
        m_Rendered++;
    }

    void RenderStats::OnEntityCulled()
    {
        m_Total++;
        m_Culled++;
    }

    void RenderStats::Update(const engine::Timestep& timestep)
    {
        m_AccumulatedTime += timestep.GetSeconds();

        if (m_AccumulatedTime >= 0.25f) // update 4x/sec
        {
            std::ostringstream oss;
            oss << "Render: "
                << "T " << m_Total
                << " | R " << m_Rendered
                << " | C " << m_Culled;

            m_DisplayString = oss.str();

            m_AccumulatedTime = 0.0f;
        }
    }
}
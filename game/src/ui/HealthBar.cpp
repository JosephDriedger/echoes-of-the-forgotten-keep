// Created by Adam Van Woerden

#include "game/ui/HealthBar.h"

namespace game
{
    void HealthBar::SetTextures(std::shared_ptr<engine::Texture> heart,
                             std::shared_ptr<engine::Texture> empty,
                             std::shared_ptr<engine::Texture> border)
    {
        m_Heart  = heart;
        m_Empty  = empty;
        m_Border = border;
    }

    void HealthBar::Render(UIRenderer& renderer, int current, int max,
                            float x, float y, float heartSize, float spacing,
                            int screenWidth, int screenHeight)
    {
        for (int i = 0; i < max; i++)
        {
            float hx = x + i * (heartSize + spacing);

            if (m_Empty)
                renderer.DrawQuad(m_Empty,  hx, y, heartSize, heartSize, screenWidth, screenHeight);
            if (i < current && m_Heart)
                renderer.DrawQuad(m_Heart,  hx, y, heartSize, heartSize, screenWidth, screenHeight);
            if (m_Border)
                renderer.DrawQuad(m_Border, hx, y, heartSize, heartSize, screenWidth, screenHeight);
        }
    }
}
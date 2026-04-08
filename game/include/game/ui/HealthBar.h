// Created by Adam Van Woerden
/// @file HealthBar.h
/// @brief Icon-based health bar rendered as a row of heart icons.
///
/// Draws one icon per max HP. Each slot layers three textures:
/// empty background, filled heart (if HP remains), and decorative border.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_HEALTHBAR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_HEALTHBAR_H

#include "game/ui/UIRenderer.h"
#include "engine/rendering/Texture.h"
#include <memory>

namespace game {
    class HealthBar {
    public:
        /// Assigns the three layered textures used for each heart slot.
        void SetTextures(std::shared_ptr<engine::Texture> heart,
                         std::shared_ptr<engine::Texture> empty,
                         std::shared_ptr<engine::Texture> border);

        /// Draws a horizontal row of heart icons starting at (x, y).
        void Render(game::UIRenderer& renderer, int current, int max,
                    float x, float y, float heartSize, float spacing,
                    int screenWidth, int screenHeight);
    private:
        std::shared_ptr<engine::Texture> m_Heart;   ///< Filled heart icon
        std::shared_ptr<engine::Texture> m_Empty;   ///< Empty/background icon
        std::shared_ptr<engine::Texture> m_Border;  ///< Border overlay icon
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_HEALTHBAR_H
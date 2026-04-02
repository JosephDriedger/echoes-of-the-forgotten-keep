// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_HEALTHBAR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_HEALTHBAR_H

#include "game/ui/UIRenderer.h"
#include "engine/rendering/Texture.h"
#include <memory>

namespace game {
    class HealthBar {
    public:
        void SetTextures(std::shared_ptr<engine::Texture> heart,
                         std::shared_ptr<engine::Texture> empty,
                         std::shared_ptr<engine::Texture> border);

        void Render(game::UIRenderer& renderer, int current, int max,
                    float x, float y, float heartSize, float spacing,
                    int screenWidth, int screenHeight);
    private:
        std::shared_ptr<engine::Texture> m_Heart;
        std::shared_ptr<engine::Texture> m_Empty;
        std::shared_ptr<engine::Texture> m_Border;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_HEALTHBAR_H
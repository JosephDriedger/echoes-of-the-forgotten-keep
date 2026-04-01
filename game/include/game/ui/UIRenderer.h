//
// Created by scept on 2026-04-01.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_UIRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_UIRENDERER_H

#include "engine/rendering/Texture.h"
#include <memory>

namespace game
{
    class UIRenderer
    {
    public:
        bool Initialize();
        void Shutdown();

        // Draw a textured quad in screen pixels
        void DrawQuad(std::shared_ptr<engine::Texture> texture,
                      float x, float y, float width, float height,
                      int screenWidth, int screenHeight);

    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_ShaderProgram = 0;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_UIRENDERER_H
// Created by Adam Van Woerden
/// @file UIRenderer.h
/// @brief Low-level OpenGL quad renderer for HUD textures (hearts, borders).
///
/// Converts pixel-space coordinates to NDC and draws a textured quad with
/// alpha blending. Used by HealthBar and HUD to render icon-based UI elements.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_UIRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_UIRENDERER_H

#include "engine/rendering/Texture.h"
#include <memory>

namespace game
{
    class UIRenderer
    {
    public:
        /// Compiles the internal quad shader and creates a unit-quad VAO/VBO.
        bool Initialize();
        /// Frees GPU resources (VAO, VBO, shader program).
        void Shutdown();

        /// Draws a textured quad at pixel position (x, y) with given size.
        /// Coordinates use top-left origin; conversion to NDC is handled internally.
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
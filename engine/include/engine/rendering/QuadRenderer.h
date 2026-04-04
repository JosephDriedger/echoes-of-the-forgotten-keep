// Created by Adam Van Woerden
/// @file QuadRenderer.h
/// @brief Draws filled rectangles for UI elements (slider tracks, handles, etc.).
///
/// Uses the same orthographic projection pattern as TextRenderer.
/// Temporarily disables depth testing and face culling while drawing,
/// then restores the previous GL state.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_QUADRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_QUADRENDERER_H

#include <memory>
#include <string>

namespace engine
{
    class Shader;

    /// Renders solid-color rectangles in screen space.
    class QuadRenderer
    {
    public:
        QuadRenderer();
        ~QuadRenderer();

        QuadRenderer(const QuadRenderer&) = delete;
        QuadRenderer& operator=(const QuadRenderer&) = delete;

        /// Compile the quad shaders and set up the orthographic projection.
        bool Initialize(const std::string& vertShaderPath,
                        const std::string& fragShaderPath,
                        int screenWidth, int screenHeight);

        /// Release shaders and vertex buffers.
        void Destroy();

        /// Draw a filled rectangle at (x, y) with the given size and RGBA color.
        /// Coordinates are in screen-space pixels (top-left origin).
        void DrawFilledRect(float x, float y, float width, float height,
                            float r, float g, float b, float a = 1.0f);

    private:
        std::shared_ptr<Shader> m_Shader;
        unsigned int m_VAO;
        unsigned int m_VBO;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_QUADRENDERER_H

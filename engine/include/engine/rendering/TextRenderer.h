/// @file TextRenderer.h
/// @brief TrueType font rendering using stb_truetype and OpenGL.
///
/// Bakes a font atlas from a .ttf file at initialization, then renders
/// text as textured quads using an orthographic projection. Designed
/// for UI overlays -- temporarily disables depth testing and face
/// culling while drawing, then restores the previous GL state.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTRENDERER_H

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    class Shader;

    /// Per-glyph metrics and texture-atlas coordinates.
    struct GlyphInfo
    {
        float TexX0, TexY0; ///< Top-left UV in the atlas
        float TexX1, TexY1; ///< Bottom-right UV in the atlas
        int Width, Height;  ///< Glyph bitmap size in pixels
        int OffsetX, OffsetY; ///< Bearing from the pen position
        float Advance;      ///< Horizontal advance to the next glyph
    };

    /// Renders text strings to the screen using a baked font atlas.
    ///
    /// Usage:
    ///   1. Call Initialize() once with font path, size, and shader paths.
    ///   2. Call RenderText() each frame in your OnRender().
    ///   3. Call Destroy() or let the destructor clean up GPU resources.
    class TextRenderer
    {
    public:
        TextRenderer();
        ~TextRenderer();

        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;

        /// Load a TTF font, bake a glyph atlas, and compile text shaders.
        /// @param fontPath   Path to a .ttf font file.
        /// @param fontSize   Pixel height to rasterize glyphs at.
        /// @param screenWidth, screenHeight  Used for the orthographic projection.
        /// @return true on success.
        bool Initialize(const std::string& fontPath, float fontSize,
                        const std::string& vertShaderPath,
                        const std::string& fragShaderPath,
                        int screenWidth, int screenHeight);

        /// Release the atlas texture, shaders, and vertex buffers.
        void Destroy();

        /// Draw a text string at (x, y) in screen-space pixels (top-left origin).
        void RenderText(const std::string& text, float x, float y,
                        float scale, float r, float g, float b);

        /// Measure the width in pixels a string would occupy at the given scale.
        float MeasureTextWidth(const std::string& text, float scale) const;

        /// Return the line height (ascent - descent + line gap) scaled.
        float GetLineHeight(float scale) const;

    private:
        /// Rasterize ASCII glyphs 32-126 into a single-channel texture atlas.
        bool BakeAtlas(const std::string& fontPath, float fontSize);

        std::shared_ptr<Shader> m_Shader;
        unsigned int m_AtlasTexture; ///< GL texture ID for the glyph atlas
        int m_AtlasWidth;
        int m_AtlasHeight;
        float m_FontSize;
        float m_Ascent;    ///< Distance from baseline to top of tallest glyph
        float m_Descent;   ///< Distance from baseline to bottom (negative)
        float m_LineGap;
        float m_ScaleFactor; ///< stb_truetype scale for the requested pixel height

        unsigned int m_VAO; ///< Vertex array for dynamic quad rendering
        unsigned int m_VBO; ///< Vertex buffer (re-uploaded per glyph)

        std::unordered_map<char, GlyphInfo> m_Glyphs;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTRENDERER_H

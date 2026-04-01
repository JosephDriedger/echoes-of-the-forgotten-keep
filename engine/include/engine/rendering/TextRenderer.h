#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTRENDERER_H

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    class Shader;

    struct GlyphInfo
    {
        float TexX0, TexY0;
        float TexX1, TexY1;
        int Width, Height;
        int OffsetX, OffsetY;
        float Advance;
    };

    class TextRenderer
    {
    public:
        TextRenderer();
        ~TextRenderer();

        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;

        bool Initialize(const std::string& fontPath, float fontSize,
                        const std::string& vertShaderPath,
                        const std::string& fragShaderPath,
                        int screenWidth, int screenHeight);

        void Destroy();

        void RenderText(const std::string& text, float x, float y,
                        float scale, float r, float g, float b);

        float MeasureTextWidth(const std::string& text, float scale) const;
        float GetLineHeight(float scale) const;

    private:
        bool BakeAtlas(const std::string& fontPath, float fontSize);

        std::shared_ptr<Shader> m_Shader;
        unsigned int m_AtlasTexture;
        int m_AtlasWidth;
        int m_AtlasHeight;
        float m_FontSize;
        float m_Ascent;
        float m_Descent;
        float m_LineGap;
        float m_ScaleFactor;

        unsigned int m_VAO;
        unsigned int m_VBO;

        std::unordered_map<char, GlyphInfo> m_Glyphs;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTRENDERER_H

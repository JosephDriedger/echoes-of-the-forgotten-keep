#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#include "engine/rendering/TextRenderer.h"
#include "engine/rendering/Shader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

namespace engine
{
    TextRenderer::TextRenderer()
        : m_Shader(nullptr),
          m_AtlasTexture(0),
          m_AtlasWidth(0),
          m_AtlasHeight(0),
          m_FontSize(0.0f),
          m_Ascent(0.0f),
          m_Descent(0.0f),
          m_LineGap(0.0f),
          m_ScaleFactor(0.0f),
          m_VAO(0),
          m_VBO(0)
    {
    }

    TextRenderer::~TextRenderer()
    {
        Destroy();
    }

    bool TextRenderer::Initialize(const std::string& fontPath, float fontSize,
                                  const std::string& vertShaderPath,
                                  const std::string& fragShaderPath,
                                  int screenWidth, int screenHeight)
    {
        m_Shader = std::make_shared<Shader>();
        if (!m_Shader->LoadFromFiles(vertShaderPath, fragShaderPath))
        {
            std::cerr << "[TextRenderer] Failed to load text shaders\n";
            return false;
        }

        if (!BakeAtlas(fontPath, fontSize))
        {
            std::cerr << "[TextRenderer] Failed to bake font atlas\n";
            return false;
        }

        // Set up orthographic projection
        m_Shader->Bind();
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth),
                                           static_cast<float>(screenHeight), 0.0f,
                                           -1.0f, 1.0f);
        m_Shader->SetMat4("projection", glm::value_ptr(projection));
        m_Shader->SetInt("textAtlas", 0);
        m_Shader->Unbind();

        // Create VAO/VBO for dynamic quad rendering
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        // 6 vertices * 4 floats (x, y, u, v)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              reinterpret_cast<void*>(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    void TextRenderer::Destroy()
    {
        if (m_AtlasTexture != 0)
        {
            glDeleteTextures(1, &m_AtlasTexture);
            m_AtlasTexture = 0;
        }

        if (m_VBO != 0)
        {
            glDeleteBuffers(1, &m_VBO);
            m_VBO = 0;
        }

        if (m_VAO != 0)
        {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }

        m_Shader.reset();
        m_Glyphs.clear();
    }

    bool TextRenderer::BakeAtlas(const std::string& fontPath, float fontSize)
    {
        // Read font file
        std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::cerr << "[TextRenderer] Failed to open font: " << fontPath << "\n";
            return false;
        }

        auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> fontBuffer(static_cast<size_t>(fileSize));
        file.read(reinterpret_cast<char*>(fontBuffer.data()), fileSize);
        file.close();

        // Initialize stb_truetype
        stbtt_fontinfo fontInfo;
        if (!stbtt_InitFont(&fontInfo, fontBuffer.data(), 0))
        {
            std::cerr << "[TextRenderer] Failed to init font\n";
            return false;
        }

        m_FontSize = fontSize;
        m_ScaleFactor = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);

        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
        m_Ascent = ascent * m_ScaleFactor;
        m_Descent = descent * m_ScaleFactor;
        m_LineGap = lineGap * m_ScaleFactor;

        // Bake glyphs for printable ASCII (32-126)
        m_AtlasWidth = 512;
        m_AtlasHeight = 512;
        std::vector<unsigned char> atlasData(m_AtlasWidth * m_AtlasHeight, 0);

        int penX = 1;
        int penY = 1;
        int rowHeight = 0;

        for (char c = 32; c < 127; c++)
        {
            int w, h, xoff, yoff;
            unsigned char* bitmap = stbtt_GetCodepointBitmap(
                &fontInfo, 0, m_ScaleFactor, c, &w, &h, &xoff, &yoff);

            if (penX + w + 1 >= m_AtlasWidth)
            {
                penX = 1;
                penY += rowHeight + 1;
                rowHeight = 0;
            }

            if (penY + h + 1 >= m_AtlasHeight)
            {
                std::cerr << "[TextRenderer] Atlas too small for font size\n";
                stbtt_FreeBitmap(bitmap, nullptr);
                return false;
            }

            // Copy glyph bitmap into atlas
            for (int row = 0; row < h; row++)
            {
                for (int col = 0; col < w; col++)
                {
                    atlasData[(penY + row) * m_AtlasWidth + (penX + col)] =
                        bitmap[row * w + col];
                }
            }

            GlyphInfo glyph{};
            glyph.TexX0 = static_cast<float>(penX) / m_AtlasWidth;
            glyph.TexY0 = static_cast<float>(penY) / m_AtlasHeight;
            glyph.TexX1 = static_cast<float>(penX + w) / m_AtlasWidth;
            glyph.TexY1 = static_cast<float>(penY + h) / m_AtlasHeight;
            glyph.Width = w;
            glyph.Height = h;
            glyph.OffsetX = xoff;
            glyph.OffsetY = yoff;

            int advanceWidth, leftSideBearing;
            stbtt_GetCodepointHMetrics(&fontInfo, c, &advanceWidth, &leftSideBearing);
            glyph.Advance = advanceWidth * m_ScaleFactor;

            m_Glyphs[c] = glyph;

            penX += w + 1;
            if (h > rowHeight) rowHeight = h;

            stbtt_FreeBitmap(bitmap, nullptr);
        }

        // Upload atlas to GPU
        glGenTextures(1, &m_AtlasTexture);
        glBindTexture(GL_TEXTURE_2D, m_AtlasTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_AtlasWidth, m_AtlasHeight,
                     0, GL_RED, GL_UNSIGNED_BYTE, atlasData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::cout << "[TextRenderer] Font atlas baked: " << m_AtlasWidth << "x"
                  << m_AtlasHeight << " (" << m_Glyphs.size() << " glyphs)\n";

        return true;
    }

    void TextRenderer::RenderText(const std::string& text, float x, float y,
                                  float scale, float r, float g, float b)
    {
        if (!m_Shader) return;

        // Save GL state
        GLboolean depthTestEnabled;
        GLboolean blendEnabled;
        GLboolean cullFaceEnabled;
        GLint blendSrc, blendDst;
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        glGetBooleanv(GL_BLEND, &blendEnabled);
        glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_Shader->Bind();
        m_Shader->SetVec3("textColor", r, g, b);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AtlasTexture);

        glBindVertexArray(m_VAO);

        float cursorX = x;
        float baselineY = y + m_Ascent * scale;

        for (char c : text)
        {
            auto it = m_Glyphs.find(c);
            if (it == m_Glyphs.end()) continue;

            const GlyphInfo& g_info = it->second;

            float xpos = cursorX + g_info.OffsetX * scale;
            float ypos = baselineY + g_info.OffsetY * scale;
            float w = g_info.Width * scale;
            float h = g_info.Height * scale;

            float vertices[6][4] = {
                { xpos,     ypos,     g_info.TexX0, g_info.TexY0 },
                { xpos + w, ypos,     g_info.TexX1, g_info.TexY0 },
                { xpos + w, ypos + h, g_info.TexX1, g_info.TexY1 },

                { xpos,     ypos,     g_info.TexX0, g_info.TexY0 },
                { xpos + w, ypos + h, g_info.TexX1, g_info.TexY1 },
                { xpos,     ypos + h, g_info.TexX0, g_info.TexY1 },
            };

            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            cursorX += g_info.Advance * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_Shader->Unbind();

        // Restore GL state
        if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
        if (cullFaceEnabled) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
        if (!blendEnabled) glDisable(GL_BLEND);
        glBlendFunc(blendSrc, blendDst);
    }

    float TextRenderer::MeasureTextWidth(const std::string& text, float scale) const
    {
        float width = 0.0f;
        for (char c : text)
        {
            auto it = m_Glyphs.find(c);
            if (it != m_Glyphs.end())
            {
                width += it->second.Advance * scale;
            }
        }
        return width;
    }

    float TextRenderer::GetLineHeight(float scale) const
    {
        return (m_Ascent - m_Descent + m_LineGap) * scale;
    }
}

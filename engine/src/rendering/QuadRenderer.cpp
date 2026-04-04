// Created by Adam Van Woerden
/// @file QuadRenderer.cpp
/// @brief Solid-color rectangle rendering for UI elements.

#include "engine/rendering/QuadRenderer.h"
#include "engine/rendering/Shader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

namespace engine
{
    QuadRenderer::QuadRenderer()
        : m_Shader(nullptr),
          m_VAO(0),
          m_VBO(0)
    {
    }

    QuadRenderer::~QuadRenderer()
    {
        Destroy();
    }

    bool QuadRenderer::Initialize(const std::string& vertShaderPath,
                                  const std::string& fragShaderPath,
                                  int screenWidth, int screenHeight)
    {
        m_Shader = std::make_shared<Shader>();
        if (!m_Shader->LoadFromFiles(vertShaderPath, fragShaderPath))
        {
            std::cerr << "[QuadRenderer] Failed to load shaders\n";
            return false;
        }

        m_Shader->Bind();
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth),
                                           static_cast<float>(screenHeight), 0.0f,
                                           -1.0f, 1.0f);
        m_Shader->SetMat4("projection", glm::value_ptr(projection));
        m_Shader->Unbind();

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        // 6 vertices * 2 floats (x, y)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    void QuadRenderer::Destroy()
    {
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
    }

    void QuadRenderer::DrawFilledRect(float x, float y, float width, float height,
                                      float r, float g, float b, float a)
    {
        if (!m_Shader) return;

        GLboolean depthTestEnabled;
        GLboolean cullFaceEnabled;
        GLboolean blendEnabled;
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);
        glGetBooleanv(GL_BLEND, &blendEnabled);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_Shader->Bind();
        int colorLoc = m_Shader->GetCachedUniformLocation("quadColor");
        glUniform4f(colorLoc, r, g, b, a);

        float vertices[6][2] = {
            { x,         y          },
            { x + width, y          },
            { x + width, y + height },

            { x,         y          },
            { x + width, y + height },
            { x,         y + height },
        };

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        m_Shader->Unbind();

        if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
        if (cullFaceEnabled) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
        if (!blendEnabled) glDisable(GL_BLEND);
    }
}

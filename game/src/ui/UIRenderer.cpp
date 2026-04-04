// Created by Adam Van Woerden

#include "game/ui/UIRenderer.h"

#include <glad/gl.h>
#include <iostream>

namespace game
{
    static const char* s_VertSrc = R"(
        #version 460 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aUV;
        out vec2 vUV;
        uniform vec4 uRect;     // x, y, w, h in NDC
        void main()
        {
            vec2 pos = vec2(uRect.x + aPos.x * uRect.z,
                            uRect.y + aPos.y * uRect.w);
            gl_Position = vec4(pos, 0.0, 1.0);
            vUV = aUV;
        }
    )";

    static const char* s_FragSrc = R"(
        #version 460 core
        in vec2 vUV;
        out vec4 FragColor;
        uniform sampler2D uTex;
        void main()
        {
            FragColor = texture(uTex, vUV);
        }
    )";

    static unsigned int CompileShader(GLenum type, const char* src)
    {
        unsigned int id = glCreateShader(type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int ok;
        glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            char log[512];
            glGetShaderInfoLog(id, 512, nullptr, log);
            std::cerr << "[UIRenderer] Shader error: " << log << "\n";
        }
        return id;
    }

    bool UIRenderer::Initialize()
    {
        // Unit quad: two triangles covering [0,1] x [0,1]
        float verts[] = {
            // pos      // uv
            0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, 0.0f,  1.0f, 1.0f,
            1.0f, 1.0f,  1.0f, 0.0f,

            0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, 1.0f,  1.0f, 0.0f,
            0.0f, 1.0f,  0.0f, 0.0f,
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);

        unsigned int vert = CompileShader(GL_VERTEX_SHADER,   s_VertSrc);
        unsigned int frag = CompileShader(GL_FRAGMENT_SHADER, s_FragSrc);

        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vert);
        glAttachShader(m_ShaderProgram, frag);
        glLinkProgram(m_ShaderProgram);
        glDeleteShader(vert);
        glDeleteShader(frag);

        return true;
    }

    void UIRenderer::Shutdown()
    {
        if (m_VAO) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
        if (m_VBO) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
        if (m_ShaderProgram) { glDeleteProgram(m_ShaderProgram); m_ShaderProgram = 0; }
    }

    void UIRenderer::DrawQuad(std::shared_ptr<engine::Texture> texture,
                               float x, float y, float width, float height,
                               int screenWidth, int screenHeight)
    {
        if (!texture || !texture->IsLoaded()) return;

        // Convert pixel coords to NDC: top-left origin → GL bottom-left origin
        float ndcX =  (x / screenWidth)  * 2.0f - 1.0f;
        float ndcY = -(y / screenHeight) * 2.0f + 1.0f - (height / screenHeight) * 2.0f;
        float ndcW =  (width  / screenWidth)  * 2.0f;
        float ndcH =  (height / screenHeight) * 2.0f;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(m_ShaderProgram);
        glUniform4f(glGetUniformLocation(m_ShaderProgram, "uRect"), ndcX, ndcY, ndcW, ndcH);
        glUniform1i(glGetUniformLocation(m_ShaderProgram, "uTex"), 0);

        texture->Bind(0);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}
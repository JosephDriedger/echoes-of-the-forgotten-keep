// Created by Elijah Fabon

#include "game/systems/DebugColliderRenderer.h"

#include "game/components/Components.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace game
{
    static const char* kDebugVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    static const char* kDebugFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 lineColor;

        void main()
        {
            FragColor = vec4(lineColor, 1.0);
        }
    )";

    // 12 edges of a unit cube (0,0,0) to (1,1,1)
    static const float kCubeLines[] = {
        // Bottom face
        0,0,0, 1,0,0,
        1,0,0, 1,0,1,
        1,0,1, 0,0,1,
        0,0,1, 0,0,0,
        // Top face
        0,1,0, 1,1,0,
        1,1,0, 1,1,1,
        1,1,1, 0,1,1,
        0,1,1, 0,1,0,
        // Vertical edges
        0,0,0, 0,1,0,
        1,0,0, 1,1,0,
        1,0,1, 1,1,1,
        0,0,1, 0,1,1,
    };

    DebugColliderRenderer::DebugColliderRenderer() = default;

    DebugColliderRenderer::~DebugColliderRenderer()
    {
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_ShaderProgram) glDeleteProgram(m_ShaderProgram);
    }

    bool DebugColliderRenderer::Initialize()
    {
        // Compile vertex shader
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &kDebugVertexShader, nullptr);
        glCompileShader(vs);

        int success;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(vs, 512, nullptr, log);
            std::cerr << "[DebugColliderRenderer] Vertex shader error: " << log << "\n";
            return false;
        }

        // Compile fragment shader
        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &kDebugFragmentShader, nullptr);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(fs, 512, nullptr, log);
            std::cerr << "[DebugColliderRenderer] Fragment shader error: " << log << "\n";
            return false;
        }

        // Link program
        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vs);
        glAttachShader(m_ShaderProgram, fs);
        glLinkProgram(m_ShaderProgram);

        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, log);
            std::cerr << "[DebugColliderRenderer] Shader link error: " << log << "\n";
            return false;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        // Create VAO/VBO for wireframe cube
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeLines), kCubeLines, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        m_Initialized = true;
        return true;
    }

    void DebugColliderRenderer::Render(const engine::Registry& registry, const engine::Camera& camera)
    {
        if (!m_Initialized)
            return;

        glUseProgram(m_ShaderProgram);

        int viewLoc = glGetUniformLocation(m_ShaderProgram, "view");
        int projLoc = glGetUniformLocation(m_ShaderProgram, "projection");
        int modelLoc = glGetUniformLocation(m_ShaderProgram, "model");
        int colorLoc = glGetUniformLocation(m_ShaderProgram, "lineColor");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, camera.GetViewMatrix());
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, camera.GetProjectionMatrix());

        glDisable(GL_DEPTH_TEST);
        glLineWidth(2.0f);

        glBindVertexArray(m_VAO);

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Transform>(entity) ||
                !registry.HasComponent<Collider>(entity))
            {
                continue;
            }

            const auto& transform = registry.GetComponent<Transform>(entity);
            const auto& collider = registry.GetComponent<Collider>(entity);

            // Collider center = transform position + offset, then expand by half-size
            float centerX = transform.X + collider.OffsetX;
            float centerY = transform.Y + collider.OffsetY;
            float centerZ = transform.Z + collider.OffsetZ;

            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(
                centerX - collider.Width * 0.5f,
                centerY,
                centerZ - collider.Depth * 0.5f));
            model = glm::scale(model, glm::vec3(collider.Width, collider.Height, collider.Depth));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Green for player, red for triggers, yellow for normal colliders
            if (registry.HasComponent<Player>(entity))
                glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);
            else if (collider.IsTrigger)
                glUniform3f(colorLoc, 0.0f, 0.5f, 1.0f);
            else
                glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f);

            glDrawArrays(GL_LINES, 0, 24);
        }

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }
}

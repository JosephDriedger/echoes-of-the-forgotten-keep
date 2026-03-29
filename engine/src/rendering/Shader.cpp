//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/rendering/Shader.h"

#include <glad/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace engine
{
    Shader::Shader()
        : m_ProgramId(0),
          m_IsLoaded(false)
    {
    }

    Shader::~Shader()
    {
        Destroy();
    }

    Shader::Shader(Shader&& other) noexcept
        : m_ProgramId(other.m_ProgramId),
          m_IsLoaded(other.m_IsLoaded)
    {
        other.m_ProgramId = 0;
        other.m_IsLoaded = false;
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        Destroy();

        m_ProgramId = other.m_ProgramId;
        m_IsLoaded = other.m_IsLoaded;

        other.m_ProgramId = 0;
        other.m_IsLoaded = false;

        return *this;
    }

    bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
    {
        Destroy();

        const std::string vertexSource = ReadTextFile(vertexPath);
        const std::string fragmentSource = ReadTextFile(fragmentPath);

        if (vertexSource.empty() || fragmentSource.empty())
        {
            std::cerr << "[Shader] Failed to read shader source files.\n";
            return false;
        }

        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        if (!CompileStage(vertexShader, vertexSource.c_str(), vertexPath.c_str()))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        if (!CompileStage(fragmentShader, fragmentSource.c_str(), fragmentPath.c_str()))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        m_ProgramId = glCreateProgram();
        glAttachShader(m_ProgramId, vertexShader);
        glAttachShader(m_ProgramId, fragmentShader);

        const bool linked = LinkProgram(m_ProgramId);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        if (!linked)
        {
            glDeleteProgram(m_ProgramId);
            m_ProgramId = 0;
            return false;
        }

        m_IsLoaded = true;
        return true;
    }

    void Shader::Destroy()
    {
        if (m_ProgramId != 0)
        {
            glDeleteProgram(m_ProgramId);
            m_ProgramId = 0;
        }

        m_IsLoaded = false;
    }

    void Shader::Bind() const
    {
        if (m_ProgramId != 0)
        {
            glUseProgram(m_ProgramId);
        }
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetBool(const std::string& name, const bool value) const
    {
        glUniform1i(GetUniformLocation(name), value ? 1 : 0);
    }

    void Shader::SetInt(const std::string& name, const int value) const
    {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetFloat(const std::string& name, const float value) const
    {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetVec3(const std::string& name, const float x, const float y, const float z) const
    {
        glUniform3f(GetUniformLocation(name), x, y, z);
    }

    void Shader::SetMat4(const std::string& name, const float* values) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, values);
    }

    unsigned int Shader::GetProgramId() const
    {
        return m_ProgramId;
    }

    bool Shader::IsLoaded() const
    {
        return m_IsLoaded;
    }

    std::string Shader::ReadTextFile(const std::string& path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "[Shader] Could not open file: " << path << '\n';
            return {};
        }

        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    bool Shader::CompileStage(const unsigned int shaderId, const char* source, const char* label)
    {
        glShaderSource(shaderId, 1, &source, nullptr);
        glCompileShader(shaderId);

        int success = 0;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

        if (success == GL_TRUE)
        {
            return true;
        }

        char infoLog[1024] = {};
        glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);

        std::cerr << "[Shader] Compile failed for " << label << ":\n" << infoLog << '\n';
        return false;
    }

    bool Shader::LinkProgram(const unsigned int programId)
    {
        glLinkProgram(programId);

        int success = 0;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);

        if (success == GL_TRUE)
        {
            return true;
        }

        char infoLog[1024] = {};
        glGetProgramInfoLog(programId, sizeof(infoLog), nullptr, infoLog);

        std::cerr << "[Shader] Program link failed:\n" << infoLog << '\n';
        return false;
    }

    int Shader::GetUniformLocation(const std::string& name) const
    {
        return glGetUniformLocation(m_ProgramId, name.c_str());
    }
}
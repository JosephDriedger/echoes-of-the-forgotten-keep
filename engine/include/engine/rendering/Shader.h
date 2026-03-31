//
// Created by Adam Van Woerden on 3/24/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SHADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SHADER_H

#include <string>
#include <unordered_map>

namespace engine
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
        void Destroy();

        void Bind() const;
        void Unbind() const;

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec3(const std::string& name, float x, float y, float z) const;
        void SetMat4(const std::string& name, const float* values) const;
        void SetMat4(int location, const float* values) const;

        [[nodiscard]] unsigned int GetProgramId() const;
        [[nodiscard]] bool IsLoaded() const;
        [[nodiscard]] int GetCachedUniformLocation(const std::string& name) const;

    private:
        static std::string ReadTextFile(const std::string& path);
        static bool CompileStage(unsigned int shaderId, const char* source, const char* label);
        static bool LinkProgram(unsigned int programId);

        int GetUniformLocation(const std::string& name) const;

    private:
        unsigned int m_ProgramId;
        bool m_IsLoaded;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SHADER_H
// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SHADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SHADER_H

#include <string>
#include <unordered_map>

namespace engine
{
    // Wraps an OpenGL shader program (vertex + fragment stages).
    // Handles compilation, linking, uniform uploads, and GPU resource cleanup.
    // Non-copyable; ownership transfers via move semantics.
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
        // Overload that bypasses name lookup when the location is already known.
        void SetMat4(int location, const float* values) const;

        [[nodiscard]] unsigned int GetProgramId() const;
        [[nodiscard]] bool IsLoaded() const;
        [[nodiscard]] int GetCachedUniformLocation(const std::string& name) const;

    private:
        static std::string ReadTextFile(const std::string& path);
        static bool CompileStage(unsigned int shaderId, const char* source, const char* label);
        static bool LinkProgram(unsigned int programId);

        // Looks up and caches uniform locations to avoid repeated GL queries.
        int GetUniformLocation(const std::string& name) const;

    private:
        unsigned int m_ProgramId;
        bool m_IsLoaded;
        // Mutable so uniform locations can be cached even from const setter methods.
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SHADER_H
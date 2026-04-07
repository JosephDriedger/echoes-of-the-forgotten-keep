// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SHADERMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SHADERMANAGER_H

#include "engine/rendering/Shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    // Caches compiled shader programs keyed by a user-defined name.
    // Loads each shader at most once; subsequent calls return the cached instance.
    class ShaderManager
    {
    public:
        // Loads and compiles a shader from vertex/fragment source files.
        // Returns the cached instance if the key already exists.
        std::shared_ptr<Shader> Load(
            const std::string& key,
            const std::string& vertexPath,
            const std::string& fragmentPath);

        // Retrieves a previously loaded shader, or nullptr if not found.
        std::shared_ptr<Shader> Get(const std::string& key) const;

        void Clear();

    private:
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SHADERMANAGER_H
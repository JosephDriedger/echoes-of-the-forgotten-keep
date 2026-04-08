// Created by Joey Driedger

#include "engine/resources/ShaderManager.h"

#include <iostream>

namespace engine
{
    std::shared_ptr<Shader> ShaderManager::Load(
        const std::string& key,
        const std::string& vertexPath,
        const std::string& fragmentPath)
    {
        // Return the cached shader if it was already loaded under this key.
        const auto existing = m_Shaders.find(key);

        if (existing != m_Shaders.end())
        {
            return existing->second;
        }

        std::shared_ptr<Shader> shader = std::make_shared<Shader>();

        if (!shader->LoadFromFiles(vertexPath, fragmentPath))
        {
            std::cerr << "[ShaderManager] Failed to load shader: " << key << '\n';
            return nullptr;
        }

        m_Shaders.emplace(key, shader);
        return shader;
    }

    std::shared_ptr<Shader> ShaderManager::Get(const std::string& key) const
    {
        const auto it = m_Shaders.find(key);

        if (it == m_Shaders.end())
        {
            return nullptr;
        }

        return it->second;
    }

    void ShaderManager::Clear()
    {
        m_Shaders.clear();
    }
}
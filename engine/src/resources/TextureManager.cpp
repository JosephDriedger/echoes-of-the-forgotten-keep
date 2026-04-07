// Created by Joey Driedger

#include "engine/resources/TextureManager.h"

#include <iostream>

namespace engine
{
    std::shared_ptr<Texture> TextureManager::Load(const std::string& path)
    {
        // Return the cached texture if it was already loaded from this path.
        const auto existing = m_Textures.find(path);

        if (existing != m_Textures.end())
        {
            return existing->second;
        }

        std::shared_ptr<Texture> texture = std::make_shared<Texture>();

        if (!texture->LoadFromFile(path))
        {
            std::cerr << "[TextureManager] Failed to load texture: " << path << '\n';
            return nullptr;
        }

        m_Textures.emplace(path, texture);
        return texture;
    }

    std::shared_ptr<Texture> TextureManager::Get(const std::string& path) const
    {
        const auto it = m_Textures.find(path);

        if (it == m_Textures.end())
        {
            return nullptr;
        }

        return it->second;
    }

    void TextureManager::Clear()
    {
        m_Textures.clear();
    }
}
// Created by Joey Driedger

#include "engine/resources/AssetManager.h"

namespace engine
{
    ShaderManager& AssetManager::GetShaderManager()
    {
        return m_ShaderManager;
    }

    TextureManager& AssetManager::GetTextureManager()
    {
        return m_TextureManager;
    }

    void AssetManager::Clear()
    {
        m_ShaderManager.Clear();
        m_TextureManager.Clear();
    }
}
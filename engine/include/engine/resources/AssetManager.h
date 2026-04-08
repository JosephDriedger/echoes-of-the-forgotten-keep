// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ASSETMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ASSETMANAGER_H

#include "engine/resources/ShaderManager.h"
#include "engine/resources/TextureManager.h"

namespace engine
{
    // Central facade for all asset sub-managers (shaders, textures).
    // Owns each sub-manager and provides a single point for bulk cleanup.
    class AssetManager
    {
    public:
        ShaderManager& GetShaderManager();
        TextureManager& GetTextureManager();

        // Releases all cached assets across every sub-manager.
        void Clear();

    private:
        ShaderManager m_ShaderManager;
        TextureManager m_TextureManager;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ASSETMANAGER_H
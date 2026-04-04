// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ASSETMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ASSETMANAGER_H

#include "engine/resources/ShaderManager.h"
#include "engine/resources/TextureManager.h"

namespace engine
{
    class AssetManager
    {
    public:
        ShaderManager& GetShaderManager();
        TextureManager& GetTextureManager();

        void Clear();

    private:
        ShaderManager m_ShaderManager;
        TextureManager m_TextureManager;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ASSETMANAGER_H
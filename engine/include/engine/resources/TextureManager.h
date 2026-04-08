// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTUREMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTUREMANAGER_H

#include "engine/rendering/Texture.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    // Caches GPU textures keyed by file path.
    // Uses the file path as both the lookup key and the load source.
    class TextureManager
    {
    public:
        // Loads a texture from disk, or returns the cached instance if already loaded.
        std::shared_ptr<Texture> Load(const std::string& path);

        // Retrieves a previously loaded texture, or nullptr if not found.
        std::shared_ptr<Texture> Get(const std::string& path) const;

        void Clear();

    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTUREMANAGER_H
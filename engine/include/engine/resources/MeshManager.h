// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESHMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESHMANAGER_H

#include "engine/resources/MeshLoader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    class MeshManager
    {
    public:
        MeshLoader::Result Load(const std::string& path);
        std::shared_ptr<Mesh> Get(const std::string& path) const;
        void Clear();

    private:
        MeshLoader m_Loader;
        std::unordered_map<std::string, MeshLoader::Result> m_Cache;
    };
}

#endif

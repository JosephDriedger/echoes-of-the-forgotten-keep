// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESHMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESHMANAGER_H

#include "engine/resources/MeshLoader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    // Caches loaded meshes (and their skeletons) keyed by file path.
    // Wraps MeshLoader to provide load-once semantics and fast lookups.
    class MeshManager
    {
    public:
        // Loads a mesh (and skeleton if present) from disk, or returns the cached result.
        MeshLoader::Result Load(const std::string& path);

        // Retrieves just the Mesh pointer for a previously loaded path.
        std::shared_ptr<Mesh> Get(const std::string& path) const;

        void Clear();

    private:
        MeshLoader m_Loader;
        std::unordered_map<std::string, MeshLoader::Result> m_Cache;
    };
}

#endif

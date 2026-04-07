// Created by Joey Driedger

#include "engine/resources/MeshManager.h"

#include <iostream>

namespace engine
{
    // Loads a mesh from disk on first request; subsequent calls for the same
    // path return the cached result without hitting the filesystem again.
    MeshLoader::Result MeshManager::Load(const std::string& path)
    {
        auto existing = m_Cache.find(path);
        if (existing != m_Cache.end())
        {
            return existing->second;
        }

        MeshLoader::Result result = m_Loader.LoadFromFile(path);
        if (!result.MeshPtr)
        {
            std::cerr << "[MeshManager] Failed to load: " << path << '\n';
            return result;
        }

        m_Cache.emplace(path, result);
        return result;
    }

    // Returns the cached Mesh pointer, or nullptr if the path has not been loaded yet.
    std::shared_ptr<Mesh> MeshManager::Get(const std::string& path) const
    {
        auto it = m_Cache.find(path);
        if (it == m_Cache.end())
        {
            return nullptr;
        }
        return it->second.MeshPtr;
    }

    // Releases all cached meshes. Existing shared_ptrs held elsewhere remain valid.
    void MeshManager::Clear()
    {
        m_Cache.clear();
    }
}

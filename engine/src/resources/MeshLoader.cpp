//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/resources/MeshLoader.h"

#include <iostream>
#include <vector>

namespace engine
{
    std::shared_ptr<Mesh> MeshLoader::LoadQuad() const
    {
        std::vector<Vertex> vertices =
        {
            {-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f},
            { 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f},
            { 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f},
            {-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f}
        };

        std::vector<unsigned int> indices =
        {
            0, 1, 2,
            2, 3, 0
        };

        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        if (!mesh->Create(vertices, indices))
        {
            return nullptr;
        }

        return mesh;
    }

    std::shared_ptr<Mesh> MeshLoader::LoadFromFile(const std::string& path) const
    {
        std::cerr << "[MeshLoader] Stub loader used for file: " << path << '\n';
        return LoadQuad();
    }
}
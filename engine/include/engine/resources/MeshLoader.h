// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H

#include "engine/rendering/Mesh.h"
#include "engine/rendering/AnimationData.h"

#include <memory>
#include <string>

namespace engine
{
    // Loads 3D meshes and skeletal data from model files via Assimp.
    // Produces GPU-ready vertex/index data and an optional bone hierarchy.
    class MeshLoader
    {
    public:
        // Bundles the loaded mesh together with its skeleton (may be null for static meshes).
        struct Result
        {
            std::shared_ptr<Mesh> MeshPtr;
            std::shared_ptr<Skeleton> SkeletonPtr;
        };

        // Creates a simple unit quad (1x1, Z-forward) for sprite/UI rendering.
        std::shared_ptr<Mesh> LoadQuad() const;

        // Imports a model file, extracting all sub-meshes into a single Mesh
        // and building a Skeleton from any embedded bone data.
        Result LoadFromFile(const std::string& path) const;
    };
}

#endif

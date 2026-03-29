#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H

#include "engine/rendering/Mesh.h"
#include "engine/rendering/AnimationData.h"

#include <memory>
#include <string>

namespace engine
{
    class MeshLoader
    {
    public:
        struct Result
        {
            std::shared_ptr<Mesh> MeshPtr;
            std::shared_ptr<Skeleton> SkeletonPtr;
        };

        std::shared_ptr<Mesh> LoadQuad() const;
        Result LoadFromFile(const std::string& path) const;
    };
}

#endif

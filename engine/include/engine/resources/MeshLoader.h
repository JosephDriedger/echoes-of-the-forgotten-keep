//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H

#include "engine/rendering/Mesh.h"

#include <memory>
#include <string>

namespace engine
{
    class MeshLoader
    {
    public:
        std::shared_ptr<Mesh> LoadQuad() const;
        std::shared_ptr<Mesh> LoadFromFile(const std::string& path) const;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_MESHLOADER_H
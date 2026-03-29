#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_RENDER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_RENDER_H

#include "engine/rendering/Mesh.h"
#include "engine/rendering/Texture.h"

#include <memory>

namespace game
{
    struct Render
    {
        std::shared_ptr<engine::Mesh> MeshPtr;
        std::shared_ptr<engine::Texture> TexturePtr;

        Render() = default;

        Render(std::shared_ptr<engine::Mesh> mesh, std::shared_ptr<engine::Texture> texture)
            : MeshPtr(std::move(mesh)), TexturePtr(std::move(texture))
        {
        }
    };
}

#endif

//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/rendering/RenderSystem.h"

#include "engine/rendering/Mesh.h"
#include "engine/rendering/Shader.h"
#include "engine/rendering/Texture.h"

namespace engine
{
    void RenderSystem::Draw(const Mesh& mesh, const Shader& shader) const
    {
        shader.Bind();
        mesh.Draw();
    }

    void RenderSystem::Draw(const Mesh& mesh, const Shader& shader, const Texture& texture) const
    {
        shader.Bind();
        texture.Bind(0);
        mesh.Draw();
    }
}
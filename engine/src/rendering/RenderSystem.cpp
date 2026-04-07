// Created by Adam Van Woerden
/// @file RenderSystem.cpp
/// @brief Thin draw-call dispatcher that binds GPU state (shader, texture) and
///        issues the draw for a mesh. Centralizes the bind-then-draw pattern so
///        callers don't need to manage GL state ordering themselves.

#include "engine/rendering/RenderSystem.h"

#include "engine/rendering/Mesh.h"
#include "engine/rendering/Shader.h"
#include "engine/rendering/Texture.h"

namespace engine
{
    // Untextured draw: bind only the shader program, then issue the mesh draw call.
    void RenderSystem::Draw(const Mesh& mesh, const Shader& shader) const
    {
        shader.Bind();
        mesh.Draw();
    }

    // Textured draw: bind the shader, bind the diffuse texture to unit 0, then draw.
    void RenderSystem::Draw(const Mesh& mesh, const Shader& shader, const Texture& texture) const
    {
        shader.Bind();
        texture.Bind(0);
        mesh.Draw();
    }
}
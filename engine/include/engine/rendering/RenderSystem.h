//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H

namespace engine
{
    class Mesh;
    class Shader;
    class Texture;

    class RenderSystem
    {
    public:
        void Draw(const Mesh& mesh, const Shader& shader) const;
        void Draw(const Mesh& mesh, const Shader& shader, const Texture& texture) const;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H
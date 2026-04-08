// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H

namespace engine
{
    class Mesh;
    class Shader;
    class Texture;

    // Convenience layer that binds a shader (and optionally a texture) then
    // issues a draw call for the given mesh. Keeps draw-call logic centralized.
    class RenderSystem
    {
    public:
        void Draw(const Mesh& mesh, const Shader& shader) const;
        // Binds the texture to slot 0 before drawing.
        void Draw(const Mesh& mesh, const Shader& shader, const Texture& texture) const;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H
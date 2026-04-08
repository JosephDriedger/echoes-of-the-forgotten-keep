// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H

#include "engine/rendering/AnimationData.h"

#include <vector>

namespace engine
{
    // Interleaved vertex data matching the VAO layout:
    //   location 0: position (vec3)
    //   location 1: normal   (vec3)
    //   location 2: texcoord (vec2)
    //   location 3: bone IDs (ivec4) -- for GPU skinning
    //   location 4: bone weights (vec4) -- for GPU skinning
    struct Vertex
    {
        float PositionX = 0.0f;
        float PositionY = 0.0f;
        float PositionZ = 0.0f;

        float NormalX = 0.0f;
        float NormalY = 0.0f;
        float NormalZ = 0.0f;

        float TextureU = 0.0f;
        float TextureV = 0.0f;

        // Up to MAX_BONE_INFLUENCE bones per vertex; -1 means unused slot.
        int BoneIDs[MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
        float Weights[MAX_BONE_INFLUENCE] = {0.0f, 0.0f, 0.0f, 0.0f};
    };

    // Manages an indexed OpenGL mesh (VAO + VBO + EBO).
    // Uploads vertex/index data to the GPU and issues indexed draw calls.
    // Non-copyable; ownership transfers via move semantics.
    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        bool Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void Destroy();
        void Draw() const;

        [[nodiscard]] bool IsCreated() const;

    private:
        unsigned int m_VertexArrayId;
        unsigned int m_VertexBufferId;
        unsigned int m_IndexBufferId;
        unsigned int m_IndexCount;
        bool m_IsCreated;
    };
}

#endif

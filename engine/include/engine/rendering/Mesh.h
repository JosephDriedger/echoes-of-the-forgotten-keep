#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H

#include "engine/rendering/AnimationData.h"

#include <vector>

namespace engine
{
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

        int BoneIDs[MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
        float Weights[MAX_BONE_INFLUENCE] = {0.0f, 0.0f, 0.0f, 0.0f};
    };

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

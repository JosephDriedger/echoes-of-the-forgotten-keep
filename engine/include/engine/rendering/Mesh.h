//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H

#include <vector>

namespace engine
{
    struct Vertex
    {
        float PositionX;
        float PositionY;
        float PositionZ;

        float NormalX;
        float NormalY;
        float NormalZ;

        float TextureU;
        float TextureV;
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

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_MESH_H
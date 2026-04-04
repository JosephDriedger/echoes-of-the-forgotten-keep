// Created by Adam Van Woerden

#include "engine/rendering/Mesh.h"

#include <glad/gl.h>

#include <cstddef>

namespace engine
{
    Mesh::Mesh()
        : m_VertexArrayId(0),
          m_VertexBufferId(0),
          m_IndexBufferId(0),
          m_IndexCount(0),
          m_IsCreated(false)
    {
    }

    Mesh::~Mesh()
    {
        Destroy();
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : m_VertexArrayId(other.m_VertexArrayId),
          m_VertexBufferId(other.m_VertexBufferId),
          m_IndexBufferId(other.m_IndexBufferId),
          m_IndexCount(other.m_IndexCount),
          m_IsCreated(other.m_IsCreated)
    {
        other.m_VertexArrayId = 0;
        other.m_VertexBufferId = 0;
        other.m_IndexBufferId = 0;
        other.m_IndexCount = 0;
        other.m_IsCreated = false;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        Destroy();

        m_VertexArrayId = other.m_VertexArrayId;
        m_VertexBufferId = other.m_VertexBufferId;
        m_IndexBufferId = other.m_IndexBufferId;
        m_IndexCount = other.m_IndexCount;
        m_IsCreated = other.m_IsCreated;

        other.m_VertexArrayId = 0;
        other.m_VertexBufferId = 0;
        other.m_IndexBufferId = 0;
        other.m_IndexCount = 0;
        other.m_IsCreated = false;

        return *this;
    }

    bool Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        Destroy();

        if (vertices.empty() || indices.empty())
        {
            return false;
        }

        glGenVertexArrays(1, &m_VertexArrayId);
        glGenBuffers(1, &m_VertexBufferId);
        glGenBuffers(1, &m_IndexBufferId);

        glBindVertexArray(m_VertexArrayId);

        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
            vertices.data(),
            GL_STATIC_DRAW
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
            indices.data(),
            GL_STATIC_DRAW
        );

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, PositionX)));

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, NormalX)));

        // TexCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, TextureU)));

        // Bone IDs
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, BoneIDs)));

        // Bone Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, Weights)));

        glBindVertexArray(0);

        m_IndexCount = static_cast<unsigned int>(indices.size());
        m_IsCreated = true;
        return true;
    }

    void Mesh::Destroy()
    {
        if (m_IndexBufferId != 0)
        {
            glDeleteBuffers(1, &m_IndexBufferId);
            m_IndexBufferId = 0;
        }

        if (m_VertexBufferId != 0)
        {
            glDeleteBuffers(1, &m_VertexBufferId);
            m_VertexBufferId = 0;
        }

        if (m_VertexArrayId != 0)
        {
            glDeleteVertexArrays(1, &m_VertexArrayId);
            m_VertexArrayId = 0;
        }

        m_IndexCount = 0;
        m_IsCreated = false;
    }

    void Mesh::Draw() const
    {
        if (!m_IsCreated)
        {
            return;
        }

        glBindVertexArray(m_VertexArrayId);
        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexCount), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    bool Mesh::IsCreated() const
    {
        return m_IsCreated;
    }
}

// Created by Adam Van Woerden
/// @file Material.cpp
/// @brief Bundles a shader program with its diffuse texture into a single render
///        state object. Uses shared_ptr so multiple meshes can reference the same
///        GPU resources without duplicating them.

#include "engine/rendering/Material.h"

namespace engine
{
    Material::Material()
        : m_Name(),
          m_Shader(),
          m_DiffuseTexture()
    {
    }

    void Material::SetShader(const std::shared_ptr<Shader>& shader)
    {
        m_Shader = shader;
    }

    void Material::SetDiffuseTexture(const std::shared_ptr<Texture>& texture)
    {
        m_DiffuseTexture = texture;
    }

    void Material::SetName(const std::string& name)
    {
        m_Name = name;
    }

    const std::shared_ptr<Shader>& Material::GetShader() const
    {
        return m_Shader;
    }

    const std::shared_ptr<Texture>& Material::GetDiffuseTexture() const
    {
        return m_DiffuseTexture;
    }

    const std::string& Material::GetName() const
    {
        return m_Name;
    }
}
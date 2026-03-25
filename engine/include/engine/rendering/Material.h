//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MATERIAL_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MATERIAL_H

#include <memory>
#include <string>

namespace engine
{
    class Shader;
    class Texture;

    class Material
    {
    public:
        Material();

        void SetShader(const std::shared_ptr<Shader>& shader);
        void SetDiffuseTexture(const std::shared_ptr<Texture>& texture);
        void SetName(const std::string& name);

        [[nodiscard]] const std::shared_ptr<Shader>& GetShader() const;
        [[nodiscard]] const std::shared_ptr<Texture>& GetDiffuseTexture() const;
        [[nodiscard]] const std::string& GetName() const;

    private:
        std::string m_Name;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Texture> m_DiffuseTexture;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_MATERIAL_H
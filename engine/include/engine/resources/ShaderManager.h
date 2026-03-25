//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SHADERMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SHADERMANAGER_H

#include "engine/rendering/Shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
    class ShaderManager
    {
    public:
        std::shared_ptr<Shader> Load(
            const std::string& key,
            const std::string& vertexPath,
            const std::string& fragmentPath);

        std::shared_ptr<Shader> Get(const std::string& key) const;
        void Clear();

    private:
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SHADERMANAGER_H
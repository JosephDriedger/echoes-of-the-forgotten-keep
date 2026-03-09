//
// Created by adamd on 2026-03-03.
//

#ifndef ASSIGNMENT1_SHADER_H
#define ASSIGNMENT1_SHADER_H

#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;

    // uniform helpers
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};

#endif //ASSIGNMENT1_SHADER_H
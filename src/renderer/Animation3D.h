//
// Created by adamd on 2026-03-03.
//

#ifndef ASSIGNMENT1_ANIMATION_H
#define ASSIGNMENT1_ANIMATION_H

#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>

class Animation3D
{
public:
    Animation3D() = default;
    Animation3D(const std::string& animationPath);

    float GetTicksPerSecond() const { return m_TicksPerSecond; }
    float GetDuration() const { return m_Duration; }

    const aiScene* GetScene() const { return m_Scene; }

private:
    Assimp::Importer m_Importer;
    const aiScene* m_Scene = nullptr;
    float m_Duration = 0.0f;
    float m_TicksPerSecond = 0.0f;
};

#endif //ASSIGNMENT1_ANIMATION_H
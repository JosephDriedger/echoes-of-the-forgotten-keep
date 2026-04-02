// Created by Joey Driedger

#include "engine/scene/Scene.h"

namespace engine
{
    Scene::Scene(const std::string& name)
        : m_Name(name)
    {
    }

    const std::string& Scene::GetName() const
    {
        return m_Name;
    }
}
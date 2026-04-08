// Created by Joey Driedger
//
// Scene base class implementation. Lifecycle hooks (OnCreate, OnDestroy,
// OnUpdate, OnRender) are virtual no-ops defined in the header;
// only the constructor and name accessor live here.

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
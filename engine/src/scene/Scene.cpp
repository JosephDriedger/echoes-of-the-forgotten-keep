//
// Created by Joseph Driedger on 3/17/2026.
//

#include "engine/scene/Scene.h"

#include "engine/core/Application.h"

namespace engine
{
    Scene::Scene(std::string name)
        : m_Name(std::move(name)),
          m_Registry()
    {
    }

    bool Scene::OnCreate(Application& application)
    {
        (void)application;
        return true;
    }

    void Scene::OnDestroy(Application& application)
    {
        (void)application;
    }

    void Scene::OnUpdate(Application& application, Timestep timestep)
    {
        (void)application;
        (void)timestep;
    }

    void Scene::OnRender(Application& application)
    {
        (void)application;
    }

    const std::string& Scene::GetName() const
    {
        return m_Name;
    }

    Registry& Scene::GetRegistry()
    {
        return m_Registry;
    }

    const Registry& Scene::GetRegistry() const
    {
        return m_Registry;
    }
}
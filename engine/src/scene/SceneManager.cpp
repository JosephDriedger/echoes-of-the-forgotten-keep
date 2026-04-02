// Created by Joey Driedger

#include "engine/scene/SceneManager.h"

#include "engine/scene/Scene.h"
#include "engine/core/Application.h"

namespace engine
{
    SceneManager::SceneManager()
        : m_Application(nullptr)
    {
    }

    bool SceneManager::Initialize(Application& application)
    {
        m_Application = &application;
        return true;
    }

    void SceneManager::Shutdown()
    {
        while (!m_SceneStack.empty())
        {
            m_SceneStack.back()->OnDestroy();
            m_SceneStack.pop_back();
        }

        m_Application = nullptr;
    }

    void SceneManager::PopScene()
    {
        if (m_SceneStack.empty())
            return;

        m_SceneStack.back()->OnDestroy();
        m_SceneStack.pop_back();
    }

    void SceneManager::Update(Timestep timestep)
    {
        // Only the top scene receives updates
        if (!m_SceneStack.empty())
        {
            m_SceneStack.back()->OnUpdate(*m_Application, timestep);
        }
    }

    void SceneManager::Render()
    {
        // Render all scenes bottom to top so gameplay shows behind overlays
        for (auto& scene : m_SceneStack)
        {
            scene->OnRender(*m_Application);
        }
    }
}

#include "engine/scene/SceneManager.h"

#include "engine/scene/Scene.h"
#include "engine/core/Application.h"

namespace engine
{
    SceneManager::SceneManager()
        : m_Application(nullptr),
          m_ActiveScene(nullptr)
    {
    }

    bool SceneManager::Initialize(Application& application)
    {
        m_Application = &application;
        return true;
    }

    void SceneManager::Shutdown()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnDestroy();
            m_ActiveScene.reset();
        }

        m_Application = nullptr;
    }

    void SceneManager::Update(Timestep timestep)
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnUpdate(*m_Application, timestep);
        }
    }

    void SceneManager::Render()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnRender(*m_Application);
        }
    }
}
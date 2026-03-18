//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/scene/SceneManager.h"

#include "engine/core/Application.h"
#include "engine/scene/Scene.h"

namespace engine
{
    namespace
    {
        const std::string EMPTY_SCENE_NAME;
    }

    SceneManager::SceneManager()
        : m_Application(nullptr),
          m_CurrentScene(nullptr),
          m_PendingScene(nullptr),
          m_IsInitialized(false)
    {
    }

    SceneManager::~SceneManager() = default;

    bool SceneManager::Initialize(Application& application)
    {
        if (m_IsInitialized)
        {
            return true;
        }

        m_Application = &application;
        m_IsInitialized = true;
        return true;
    }

    void SceneManager::Shutdown()
    {
        if (!m_IsInitialized)
        {
            return;
        }

        m_PendingScene.reset();

        if (m_CurrentScene && m_Application)
        {
            m_CurrentScene->OnDestroy(*m_Application);
        }

        m_CurrentScene.reset();
        m_Application = nullptr;
        m_IsInitialized = false;
    }

    void SceneManager::ChangeScene(std::unique_ptr<Scene> scene)
    {
        if (!scene)
        {
            return;
        }

        m_PendingScene = std::move(scene);
    }

    void SceneManager::Update(const Timestep timestep)
    {
        if (!m_IsInitialized || !m_Application)
        {
            return;
        }

        CommitPendingScene();

        if (!m_CurrentScene)
        {
            return;
        }

        m_CurrentScene->OnUpdate(*m_Application, timestep);

        if (m_PendingScene)
        {
            CommitPendingScene();
        }
    }

    void SceneManager::Render()
    {
        if (!m_IsInitialized || !m_Application)
        {
            return;
        }

        CommitPendingScene();

        if (!m_CurrentScene)
        {
            return;
        }

        m_CurrentScene->OnRender(*m_Application);
    }

    bool SceneManager::HasActiveScene() const
    {
        return m_CurrentScene != nullptr;
    }

    Scene* SceneManager::GetCurrentScene()
    {
        return m_CurrentScene.get();
    }

    const Scene* SceneManager::GetCurrentScene() const
    {
        return m_CurrentScene.get();
    }

    const std::string& SceneManager::GetCurrentSceneName() const
    {
        if (!m_CurrentScene)
        {
            return EMPTY_SCENE_NAME;
        }

        return m_CurrentScene->GetName();
    }

    void SceneManager::CommitPendingScene()
    {
        if (!m_PendingScene || !m_Application)
        {
            return;
        }

        if (m_CurrentScene)
        {
            m_CurrentScene->OnDestroy(*m_Application);
            m_CurrentScene.reset();
        }

        m_CurrentScene = std::move(m_PendingScene);

        if (!m_CurrentScene->OnCreate(*m_Application))
        {
            m_CurrentScene.reset();
        }
    }
}
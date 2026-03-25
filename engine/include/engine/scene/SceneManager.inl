#pragma once

#include "engine/scene/Scene.h"
#include "engine/core/Application.h"

namespace engine
{
    template<typename TScene>
    void SceneManager::ChangeScene()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnDestroy();
        }

        m_ActiveScene = std::make_shared<TScene>();

        if (m_ActiveScene)
        {
            m_ActiveScene->OnCreate(*m_Application);
        }
    }
}
#pragma once

#include "engine/scene/Scene.h"
#include "engine/core/Application.h"

#include <iostream>
#include <memory>

namespace engine
{
    template<typename TScene>
    bool SceneManager::ChangeScene()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnDestroy();
        }

        m_ActiveScene = std::make_shared<TScene>();

        if (!m_ActiveScene)
        {
            return false;
        }

        if (!m_ActiveScene->OnCreate(*m_Application))
        {
            std::cerr << "[SceneManager] Scene OnCreate() failed: "
                      << m_ActiveScene->GetName() << '\n';
            m_ActiveScene.reset();
            return false;
        }

        return true;
    }
}
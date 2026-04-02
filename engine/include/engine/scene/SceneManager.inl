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
        // Destroy all scenes in the stack (top to bottom)
        while (!m_SceneStack.empty())
        {
            m_SceneStack.back()->OnDestroy();
            m_SceneStack.pop_back();
        }

        auto scene = std::make_shared<TScene>();

        if (!scene->OnCreate(*m_Application))
        {
            std::cerr << "[SceneManager] Scene OnCreate() failed: "
                      << scene->GetName() << '\n';
            return false;
        }

        m_SceneStack.push_back(scene);
        return true;
    }

    template<typename TScene>
    bool SceneManager::PushScene()
    {
        auto scene = std::make_shared<TScene>();

        if (!scene->OnCreate(*m_Application))
        {
            std::cerr << "[SceneManager] PushScene OnCreate() failed: "
                      << scene->GetName() << '\n';
            return false;
        }

        m_SceneStack.push_back(scene);
        return true;
    }
}

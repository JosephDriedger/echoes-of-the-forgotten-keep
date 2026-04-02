// Created by Joey Driedger

#ifndef EFK_SCENE_MANAGER_H
#define EFK_SCENE_MANAGER_H

#include <memory>
#include <vector>

namespace engine
{
    class Scene;
    class Application;
    class Timestep;

    class SceneManager
    {
    public:
        SceneManager();

        bool Initialize(Application& application);
        void Shutdown();

        /// Replace the entire scene stack with a new scene.
        template<typename TScene>
        bool ChangeScene();

        /// Push an overlay scene on top of the current scene (e.g. pause menu).
        /// The scene underneath stays alive but stops receiving updates.
        template<typename TScene>
        bool PushScene();

        /// Pop the top overlay scene, resuming the one underneath.
        void PopScene();

        void Update(Timestep timestep);
        void Render();

    private:
        Application* m_Application;
        std::vector<std::shared_ptr<Scene>> m_SceneStack;
    };
}

#include "SceneManager.inl"

#endif
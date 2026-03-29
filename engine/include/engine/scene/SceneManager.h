#ifndef EFK_SCENE_MANAGER_H
#define EFK_SCENE_MANAGER_H

#include <memory>

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

        template<typename TScene>
        bool ChangeScene();

        void Update(Timestep timestep);
        void Render();

    private:
        Application* m_Application;
        std::shared_ptr<Scene> m_ActiveScene;
    };
}

#include "SceneManager.inl"

#endif
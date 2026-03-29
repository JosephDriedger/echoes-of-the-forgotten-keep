//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SCENEMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SCENEMANAGER_H

#include "engine/core/Timestep.h"

#include <memory>
#include <string>
#include <utility>

namespace engine
{
    class Application;
    class Scene;

    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        bool Initialize(Application& application);
        void Shutdown();

        void ChangeScene(std::unique_ptr<Scene> scene);

        template <typename TScene, typename... TArgs>
        void ChangeScene(TArgs&&... args)
        {
            ChangeScene(std::make_unique<TScene>(std::forward<TArgs>(args)...));
        }

        void Update(Timestep timestep);
        void Render();

        [[nodiscard]] bool HasActiveScene() const;

        Scene* GetCurrentScene();
        [[nodiscard]] const Scene* GetCurrentScene() const;
        [[nodiscard]] const std::string& GetCurrentSceneName() const;

    private:
        void CommitPendingScene();

    private:
        Application* m_Application;
        std::unique_ptr<Scene> m_CurrentScene;
        std::unique_ptr<Scene> m_PendingScene;
        bool m_IsInitialized;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SCENEMANAGER_H
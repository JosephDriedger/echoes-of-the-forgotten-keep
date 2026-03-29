//
// Created by Joseph Driedger on 3/17/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H

#include "engine/core/Timestep.h"
#include "engine/ecs/Registry.h"

#include <string>
#include <utility>

namespace engine
{
    class Application;

    class Scene
    {
    public:
        explicit Scene(std::string name = "Scene");
        virtual ~Scene() = default;

        virtual bool OnCreate(Application& application);
        virtual void OnDestroy(Application& application);
        virtual void OnUpdate(Application& application, Timestep timestep);
        virtual void OnRender(Application& application);

        [[nodiscard]] const std::string& GetName() const;

        Registry& GetRegistry();
        [[nodiscard]] const Registry& GetRegistry() const;

    private:
        std::string m_Name;
        Registry m_Registry;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H
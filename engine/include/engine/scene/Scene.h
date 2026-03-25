//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H

#include <string>

namespace engine
{
    class Application;
    class Timestep;

    class Scene
    {
    public:
        explicit Scene(const std::string& name);
        virtual ~Scene() = default;

        virtual bool OnCreate(Application& application) { return true; }
        virtual void OnDestroy() {}

        virtual void OnUpdate(Application& application, Timestep timestep) {}
        virtual void OnRender(Application& application) {}

        [[nodiscard]] const std::string& GetName() const;

    private:
        std::string m_Name;
    };
}

#endif
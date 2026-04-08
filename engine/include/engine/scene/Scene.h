// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SCENE_H

#include <string>

#include "engine/core/Timestep.h"

namespace engine
{
    class Application;

    // Abstract base class for game scenes (e.g. gameplay, menus, pause overlays).
    // Subclasses override the lifecycle hooks; the SceneManager drives them.
    class Scene
    {
    public:
        explicit Scene(const std::string& name);
        virtual ~Scene() = default;

        // Called once when the scene is pushed onto the stack. Return false to abort.
        virtual bool OnCreate(Application& application) { return true; }
        // Called when the scene is removed from the stack.
        virtual void OnDestroy() {}

        // Called each frame for the topmost scene only.
        virtual void OnUpdate(Application& application, Timestep timestep) {}
        // Called each frame for every scene in the stack (bottom to top) so
        // gameplay renders behind overlays like the pause menu.
        virtual void OnRender(Application& application) {}

        [[nodiscard]] const std::string& GetName() const;

    private:
        std::string m_Name;
    };
}

#endif
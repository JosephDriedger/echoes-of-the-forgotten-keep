// Created by Adam Van Woerden

// UISystem -- Initializes and renders the in-game HUD (health bar,
// crosshair, etc.) using the UIRenderer for 2D screen-space drawing.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_UISYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_UISYSTEM_H
#include "game/ui/HUD.h"
#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"
#include "SDL3/SDL_render.h"

namespace game {
    class UISystem {
    public:
        void Initialize(engine::AssetManager& assets);
        void Render(engine::Registry& registry, engine::Entity playerEntity,
                          int screenWidth, int screenHeight);
    private:
        HUD m_HUD;
        UIRenderer m_UIRenderer;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_UISYSTEM_H
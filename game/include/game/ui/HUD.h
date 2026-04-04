// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_HUD_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_HUD_H

#include "game/ui/HealthBar.h"
#include "game/ui/UIRenderer.h"
#include "engine/resources/AssetManager.h"
#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    class HUD
    {
    public:
        void Initialize(engine::AssetManager& assets);
        void Shutdown();
        void Render(UIRenderer& renderer, engine::Registry& registry,
                    engine::Entity playerEntity, int screenWidth, int screenHeight);
    private:
        HealthBar m_HealthBar;
    };
}

#endif
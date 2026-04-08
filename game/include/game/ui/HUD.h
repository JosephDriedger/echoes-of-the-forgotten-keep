// Created by Adam Van Woerden
/// @file HUD.h
/// @brief In-game heads-up display drawn over the 3D scene.
///
/// Currently contains the player health bar. Additional HUD elements
/// (minimap, status effects, etc.) can be added here in the future.

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
        /// Loads HUD textures (heart icons, border) from the asset manager.
        void Initialize(engine::AssetManager& assets);
        void Shutdown();
        /// Reads the player's Health component and renders the health bar.
        void Render(UIRenderer& renderer, engine::Registry& registry,
                    engine::Entity playerEntity, int screenWidth, int screenHeight);
    private:
        HealthBar m_HealthBar;
    };
}

#endif
// Created by Adam Van Woerden
/// @file HUD.cpp
/// @brief HUD rendering -- reads player Health and delegates to HealthBar.

#include "game/ui/HUD.h"
#include "game/components/Components.h"

namespace game
{
    void HUD::Initialize(engine::AssetManager& assets)
    {
        auto heart  = assets.GetTextureManager().Load("asset/ui/heart.png");
        auto empty  = assets.GetTextureManager().Load("asset/ui/background.png");
        auto border = assets.GetTextureManager().Load("asset/ui/border.png");
        m_HealthBar.SetTextures(heart, empty, border);
    }

    void HUD::Render(UIRenderer& renderer, engine::Registry& registry,
                     engine::Entity playerEntity, int screenWidth, int screenHeight)
    {
        if (!playerEntity.IsValid() || !registry.IsAlive(playerEntity)) return;
        if (!registry.HasComponent<Health>(playerEntity)) return;

        const auto& health = registry.GetComponent<Health>(playerEntity);
        m_HealthBar.Render(renderer, health.Current, health.Maximum,
                           16.0f, 16.0f, 32.0f, 4.0f, screenWidth, screenHeight);
    }
}


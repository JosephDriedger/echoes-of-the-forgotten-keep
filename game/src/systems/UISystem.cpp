// Created by Adam Van Woerden

#include "game/systems/UISystem.h"
#include "SDL3/SDL_render.h"

namespace game
{
    void UISystem::Initialize(engine::AssetManager& assets)
    {
        m_UIRenderer.Initialize();
        m_HUD.Initialize(assets);
    }

    void UISystem::Render(engine::Registry& registry, engine::Entity playerEntity,
                          int screenWidth, int screenHeight)
    {
        m_HUD.Render(m_UIRenderer, registry, playerEntity, screenWidth, screenHeight);
    }
}

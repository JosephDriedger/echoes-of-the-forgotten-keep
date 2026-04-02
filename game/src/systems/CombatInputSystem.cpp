// Created by Elijah Fabon

#include "game/systems/CombatInputSystem.h"

#include "game/components/Components.h"

#include <SDL3/SDL_keycode.h>

namespace game
{
    void CombatInputSystem::Update(engine::Registry& registry, const engine::Input& input)
    {
        if (!input.IsKeyPressed(SDLK_SPACE))
            return;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Player>(entity) ||
                !registry.HasComponent<CombatState>(entity) ||
                !registry.HasComponent<AnimationState>(entity))
                continue;

            auto& combat = registry.GetComponent<CombatState>(entity);
            auto& anim = registry.GetComponent<AnimationState>(entity);

            if (combat.IsDead)
                continue;

            if (combat.IsAttacking)
            {
                if (combat.ComboTimer > 0.0f && combat.ComboTimer <= combat.ComboWindow)
                {
                    combat.AttackQueued = true;
                }
            }
            else
            {
                combat.IsAttacking = true;
                combat.ComboIndex = 0;
                anim.CurrentTime = 0.0f;
                if (anim.Attack1ClipIndex >= 0)
                    anim.CurrentClip = anim.Attack1ClipIndex;
            }
        }
    }
}

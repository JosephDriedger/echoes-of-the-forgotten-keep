// Created by Elijah Fabon

#include "game/systems/CombatInputSystem.h"

#include "game/components/Components.h"

#include <SDL3/SDL_keycode.h>

#include "engine/resources/AudioEventQueue.h"

namespace game
{
    void CombatInputSystem::Update(engine::Registry& registry, const engine::Input& input, engine::AudioEventQueue& audioEventQueue)
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

            // get the current clip for progress check
            const engine::AnimationClip* currentClip = nullptr;
            if (anim.Clips && anim.CurrentClip >= 0 &&
                anim.CurrentClip < (int)anim.Clips->size())
            {
                currentClip = &(*anim.Clips)[anim.CurrentClip];
            }

            if (combat.IsDead)
                continue;

            if (combat.IsAttacking)
            {
                // Past the interruptible point — accept the next attack
                float progress = (currentClip && currentClip->Duration > 0.0f) ? anim.CurrentTime / currentClip->Duration : 0.0f;
                if (progress >= 0.3f)
                {
                    combat.AttackQueued = true;
                }
            }
            else
            {
                audioEventQueue.push(std::make_unique<engine::AudioEvent>("attack1"));
                combat.IsAttacking = true;
                combat.ComboIndex = 0;
                anim.CurrentTime = 0.0f;
                if (anim.Attack1ClipIndex >= 0)
                    anim.CurrentClip = anim.Attack1ClipIndex;
            }
        }
    }
}

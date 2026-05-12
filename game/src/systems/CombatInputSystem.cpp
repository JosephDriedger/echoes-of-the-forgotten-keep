// Created by Elijah Fabon

#include "game/systems/CombatInputSystem.h"

#include "game/components/Components.h"

#include <SDL3/SDL_keycode.h>

#include "engine/resources/AudioEventQueue.h"

namespace game
{
    void CombatInputSystem::Update(engine::Registry& registry, engine::Entity playerEntity, const engine::Input& input, engine::AudioEventQueue& audioEventQueue)
    {
        if (!input.IsKeyPressed(SDLK_SPACE))
            return;


        if (!registry.HasComponent<Player>(playerEntity) ||
            !registry.HasComponent<CombatState>(playerEntity) ||
            !registry.HasComponent<AnimationState>(playerEntity))
            return;

        auto& combat = registry.GetComponent<CombatState>(playerEntity);
        auto& anim = registry.GetComponent<AnimationState>(playerEntity);

        // get the current clip for progress check
        const engine::AnimationClip* currentClip = nullptr;
        if (anim.Clips && anim.CurrentClip >= 0 &&
            anim.CurrentClip < (int)anim.Clips->size())
        {
            currentClip = &(*anim.Clips)[anim.CurrentClip];
        }

        if (combat.IsDead)
            return;

        if (combat.IsAttacking)
        {
            // Buffer next combo hit once the current swing is 30% done,
            // preventing accidental double-taps from retriggering too early
            float progress = (currentClip && currentClip->Duration > 0.0f) ? anim.CurrentTime / currentClip->Duration : 0.0f;
            if (progress >= 0.3f)
            {
                combat.AttackQueued = true;
            }
        }
        else
        {
            // Start a fresh combo from the first swing
            audioEventQueue.push(std::make_unique<engine::AudioEvent>("attack1"));
            combat.IsAttacking = true;
            combat.ComboIndex = 0;
            anim.CurrentTime = 0.0f;
            if (anim.Attack1ClipIndex >= 0)
                anim.CurrentClip = anim.Attack1ClipIndex;
        }

    }
}

// Created by Elijah Fabon

#include "game/systems/CombatInputSystem.h"

#include "game/components/Components.h"

#include <SDL3/SDL_keycode.h>

#include "engine/resources/AudioEventQueue.h"

namespace game
{
    void CombatInputSystem::Update(engine::Registry& registry, const engine::Input& input, engine::AudioEventQueue& audioEventQueue)
    {
        // if (!input.IsKeyPressed(SDLK_SPACE))
        //     return;

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

            // --- Dash: Left Shift ---
            if (input.IsKeyPressed(SDLK_LSHIFT) &&
                !combat.IsDashing && !combat.IsAttacking &&
                combat.DashCooldownTimer <= 0.0f &&
                registry.HasComponent<Transform>(entity))
            {
                const auto& tf    = registry.GetComponent<Transform>(entity);
                combat.IsDashing        = true;
                combat.DashTimer        = combat.DashDuration;
                combat.DashCooldownTimer = combat.DashCooldown;
                // combat.DashDX           = std::sin(tf.RotationY);
                // combat.DashDZ           = std::cos(tf.RotationY);

                float dashX = 0.0f;
                float dashZ = 0.0f;
                if (input.IsKeyDown(SDLK_W) || input.IsKeyDown(SDLK_UP))    dashZ -= 1.0f;
                if (input.IsKeyDown(SDLK_S) || input.IsKeyDown(SDLK_DOWN))  dashZ += 1.0f;
                if (input.IsKeyDown(SDLK_A) || input.IsKeyDown(SDLK_LEFT))  dashX -= 1.0f;
                if (input.IsKeyDown(SDLK_D) || input.IsKeyDown(SDLK_RIGHT)) dashX += 1.0f;

                // Normalize, fall back to facing direction if no input held
                float len = std::sqrt(dashX * dashX + dashZ * dashZ);
                if (len > 0.0001f) { dashX /= len; dashZ /= len; }
                else { dashX = std::sin(tf.RotationY); dashZ = std::cos(tf.RotationY); }

                combat.DashDX = dashX;
                combat.DashDZ = dashZ;

                audioEventQueue.push(std::make_unique<engine::AudioEvent>("dash"));
            }

            // --- Attack: Space (unchanged logic, just can't start while dashing) ---
            if (!input.IsKeyPressed(SDLK_SPACE) || combat.IsDashing) continue;

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
}

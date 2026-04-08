// Created by Elijah Fabon

#include "game/systems/MovementSystem.h"

#include "game/components/Components.h"

#include <SDL3/SDL_keycode.h>

#include <cmath>

namespace game
{
    void MovementSystem::Update(engine::Registry& registry, const engine::Input& input, float deltaTime)
    {
        const float speed = 6.0f;
        const float rotationSpeed = 10.0f;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Player>(entity) ||
                !registry.HasComponent<Transform>(entity))
            {
                continue;
            }

            // Check combat state to block movement during attacks/hits
            if (registry.HasComponent<CombatState>(entity))
            {
                const auto& combat = registry.GetComponent<CombatState>(entity);
                if (combat.IncomingHit.has_value() || combat.IsDead)
                {
                    if (registry.HasComponent<AnimationState>(entity))
                        registry.GetComponent<AnimationState>(entity).IsMoving = false;
                    continue;
                }
            }

            auto& transform = registry.GetComponent<Transform>(entity);

            float moveX = 0.0f;
            float moveZ = 0.0f;

            if (input.IsKeyDown(SDLK_W) || input.IsKeyDown(SDLK_UP))
                moveZ -= 1.0f;
            if (input.IsKeyDown(SDLK_S) || input.IsKeyDown(SDLK_DOWN))
                moveZ += 1.0f;
            if (input.IsKeyDown(SDLK_A) || input.IsKeyDown(SDLK_LEFT))
                moveX -= 1.0f;
            if (input.IsKeyDown(SDLK_D) || input.IsKeyDown(SDLK_RIGHT))
                moveX += 1.0f;

            // Normalize the input vector to prevent faster diagonal movement
            float length = std::sqrt(moveX * moveX + moveZ * moveZ);
            bool isMoving = length > 0.0001f;

            bool isAttacking = registry.HasComponent<CombatState>(entity) &&
                   registry.GetComponent<CombatState>(entity).IsAttacking;

            if (registry.HasComponent<AnimationState>(entity))
            {
                registry.GetComponent<AnimationState>(entity).IsMoving = isMoving && !isAttacking;
            }

            if (isMoving)
            {
                moveX /= length;
                moveZ /= length;

                // Smoothly interpolate rotation toward movement direction.
                // atan2(sin,cos) wraps the angle to [-pi,pi] to avoid
                // spinning the long way around.
                float targetYaw = std::atan2(moveX, moveZ);

                float delta = targetYaw - transform.RotationY;
                delta = std::atan2(std::sin(delta), std::cos(delta));

                transform.RotationY += delta * rotationSpeed * deltaTime;
                transform.RotationY = std::atan2(
                    std::sin(transform.RotationY),
                    std::cos(transform.RotationY));

                if (!isAttacking) {
                    transform.X += moveX * speed * deltaTime;
                    transform.Z += moveZ * speed * deltaTime;
                }
            }

            // Apply forward lunge during melee attacks (decelerates over time)
            if (isAttacking)
            {
                auto& combat = registry.GetComponent<CombatState>(entity);
                if (combat.IsLunging && combat.LungeTimer > 0.0f)
                {
                    combat.LungeTimer -= deltaTime;

                    float t = combat.LungeTimer / combat.LungeDuration; // 1->0 deceleration curve
                    transform.X += std::sin(transform.RotationY) * combat.LungeSpeed * t * deltaTime;
                    transform.Z += std::cos(transform.RotationY) * combat.LungeSpeed * t * deltaTime;

                    if (combat.LungeTimer <= 0.0f)
                        combat.IsLunging = false;
                }
            }
        }
    }
}

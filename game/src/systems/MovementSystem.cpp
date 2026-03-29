#include "game/systems/MovementSystem.h"

#include "game/components/Transform.h"
#include "game/components/Player.h"
#include "game/components/AnimationState.h"

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

            if (registry.HasComponent<AnimationState>(entity))
            {
                auto& anim = registry.GetComponent<AnimationState>(entity);
                if (anim.IsAttacking || anim.IsHit)
                {
                    anim.IsMoving = false;
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

            float length = std::sqrt(moveX * moveX + moveZ * moveZ);
            bool isMoving = length > 0.0001f;

            if (registry.HasComponent<AnimationState>(entity))
            {
                registry.GetComponent<AnimationState>(entity).IsMoving = isMoving;
            }

            if (isMoving)
            {
                moveX /= length;
                moveZ /= length;

                float targetYaw = std::atan2(moveX, moveZ);

                float delta = targetYaw - transform.RotationY;
                delta = std::atan2(std::sin(delta), std::cos(delta));

                transform.RotationY += delta * rotationSpeed * deltaTime;
                transform.RotationY = std::atan2(
                    std::sin(transform.RotationY),
                    std::cos(transform.RotationY));

                transform.X += moveX * speed * deltaTime;
                transform.Z += moveZ * speed * deltaTime;
            }
        }
    }
}

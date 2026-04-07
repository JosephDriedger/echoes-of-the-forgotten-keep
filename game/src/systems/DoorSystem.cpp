// Created by Elijah Fabon

#include "game/systems/DoorSystem.h"
#include "game/systems/DoorAnimationHelper.h"

#include "game/components/Components.h"

#include <cmath>

namespace game
{
    void DoorSystem::Update(engine::Registry& registry, float deltaTime)
    {
        // Find the player position
        float playerX = 0.0f;
        float playerZ = 0.0f;
        bool playerFound = false;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (registry.HasComponent<Player>(entity) &&
                registry.HasComponent<Transform>(entity))
            {
                const auto& playerT = registry.GetComponent<Transform>(entity);
                playerX = playerT.X;
                playerZ = playerT.Z;
                playerFound = true;
                break;
            }
        }

        if (!playerFound)
            return;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Door>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            auto& door = registry.GetComponent<Door>(entity);

            auto& doorT = registry.GetComponent<Transform>(entity);

            // Check if player is within trigger distance
            float dx = playerX - doorT.X;
            float dz = playerZ - doorT.Z;
            float distSq = dx * dx + dz * dz;
            bool playerNear = distSq < 2.5f * 2.5f;

            // Determine swing direction based on which side player approaches from
            if (playerNear && door.SwingDirection == 0)
            {
                float doorForwardX = std::sin(door.BaseRotationY);
                float doorForwardZ = std::cos(door.BaseRotationY);

                float dot = dx * doorForwardX + dz * doorForwardZ;
                door.SwingDirection = (dot > 0.0f) ? 1 : -1;
            }

            // Reset swing direction when door fully closes
            if (!playerNear && std::abs(door.CurrentAngle) < 0.1f)
            {
                door.SwingDirection = 0;
            }

            float goalAngle = playerNear ? 90.0f * door.SwingDirection : 0.0f;
            DoorAnimationHelper::AnimateDoor(registry, entity, door, doorT, goalAngle, deltaTime);
        }
    }
}

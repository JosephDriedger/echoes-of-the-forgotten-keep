//
// Created by scept on 2026-03-29.
//

#include "game/systems/DoorPuzzleSystem.h"
#include "game/components/Door.h"
#include "game/components/Switch.h"
#include "game/components/Transform.h"

#include <unordered_map>
#include <cmath>

namespace game
{
    void DoorPuzzleSystem::Update(engine::Registry& registry, float dt)
    {
        std::unordered_map<std::string, bool> triggerStates;

        // Collect switches
        for (auto entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Switch>(entity))
                continue;

            auto& sw = registry.GetComponent<Switch>(entity);
            triggerStates[sw.id] = sw.pressed;
        }

        // Update doors
        for (auto entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Door>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            auto& door = registry.GetComponent<Door>(entity);
            auto& transform = registry.GetComponent<Transform>(entity);

            bool shouldOpen = triggerStates[door.triggerId];

            if (shouldOpen != door.open)
            {
                door.open = shouldOpen;
                door.targetAngle = shouldOpen ? door.openAngle : door.closeAngle;
            }

            float diff = door.targetAngle - door.currentAngle;

            if (std::abs(diff) > 0.01f)
            {
                float step = door.speed * dt;

                if (std::abs(diff) <= step)
                    door.currentAngle = door.targetAngle;
                else
                    door.currentAngle += (diff > 0 ? step : -step);

                transform.RotationY = door.baseRotation + door.currentAngle;
            }
        }
    }
}
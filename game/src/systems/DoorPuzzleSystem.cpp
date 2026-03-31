#include "game/systems/DoorPuzzleSystem.h"

#include "game/components/Door.h"
#include "game/components/Switch.h"
#include "game/components/Transform.h"

#include <cmath>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

namespace game
{
    void DoorPuzzleSystem::Update(engine::Registry& registry, float deltaTime)
    {
        // Collect trigger states from all switch entities
        std::unordered_map<std::string, bool> triggerStates;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Switch>(entity))
                continue;

            const auto& trigger = registry.GetComponent<Switch>(entity);
            triggerStates[trigger.Id] = trigger.Pressed;
        }

        // Update doors that have a trigger ID
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Door>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            auto& door = registry.GetComponent<Door>(entity);
            auto& doorTransform = registry.GetComponent<Transform>(entity);

            // Skip doors without a trigger ID (handled by DoorSystem proximity logic)
            if (door.TriggerId.empty())
                continue;

            bool shouldOpen = false;

            auto it = triggerStates.find(door.TriggerId);
            if (it != triggerStates.end())
            {
                shouldOpen = it->second;
            }

            if (shouldOpen != door.Open)
            {
                door.Open = shouldOpen;

                if (door.Open)
                {
                    std::cout << "Door opened!" << std::endl;
                    door.TargetAngle = door.OpenAngle;
                }
                else
                {
                    std::cout << "Door closed!" << std::endl;
                    door.TargetAngle = door.CloseAngle;
                }
            }

            float diff = door.TargetAngle - door.CurrentAngle;

            if (std::abs(diff) > 0.01f)
            {
                float step = door.SwingSpeed * deltaTime;

                if (std::abs(diff) <= step)
                {
                    door.CurrentAngle = door.TargetAngle;
                }
                else
                {
                    door.CurrentAngle += (diff > 0 ? step : -step);
                }

                // Apply rotation
                doorTransform.RotationY = door.BaseRotationY + glm::radians(door.CurrentAngle);
            }
        }
    }
}

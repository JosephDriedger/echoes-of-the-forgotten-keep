//
// Created by scept on 2026-03-29.
//

#include "DoorPuzzleSystem.h"

#include <iostream>
#include <unordered_map>

#include "Component.h"

void DoorPuzzleSystem::update(std::vector<std::unique_ptr<Entity>>& entities, float dt)
{
    // collect triggers
    std::unordered_map<std::string, bool> triggerStates;

    for (auto& e : entities)
    {
        if (!e->hasComponent<Switch>()) continue;

        auto& trigger = e->getComponent<Switch>();
        triggerStates[trigger.id] = trigger.pressed;
    }

    // update doors
    for (auto& e : entities)
    {
        if (!e->hasComponent<Door>() || !e->hasComponent<Transform3D>()) continue;

        auto& door = e->getComponent<Door>();
        auto& doorTransform = e->getComponent<Transform3D>();

        bool shouldOpen = false;

        auto it = triggerStates.find(door.triggerId);
        if (it != triggerStates.end())
        {
            shouldOpen = it->second;
        }

        if (shouldOpen != door.open)
        {
            door.open = shouldOpen;

            if (door.open)
            {
                std::cout << "Door opened!" << std::endl;
                door.targetAngle = door.openAngle;
            }
            else
            {
                std::cout << "Door closed!" << std::endl;
                door.targetAngle = door.closeAngle;

                // if you removed collider earlier, you'd need to re-add it here
            }
        }

        float diff = door.targetAngle - door.currentAngle;

        if (std::abs(diff) > 0.01f)
        {
            float step = door.speed * dt;

            if (std::abs(diff) <= step)
            {
                door.currentAngle = door.targetAngle;
            }
            else
            {
                door.currentAngle += (diff > 0 ? step : -step);
            }

            // apply rotation
            doorTransform.rotation.y = door.baseRotation + door.currentAngle;
        }
    }
}

//
// Created by scept on 2026-03-29.
//

#include "DoorPuzzleSystem.h"

#include <iostream>
#include <unordered_map>

#include "Component.h"

void DoorPuzzleSystem::update(std::vector<std::unique_ptr<Entity>>& entities)
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
        if (!e->hasComponent<Door>()) continue;

        auto& door = e->getComponent<Door>();

        auto it = triggerStates.find(door.triggerId);
        if (it != triggerStates.end() && it->second)
        {
            if (!door.open)
            {
                door.open = true;
                std::cout << "Door opened!" << std::endl;

                // disable collider or move door
                if (e->hasComponent<Collider3D>())
                {
                    // simplest: remove collider
                    // e->removeComponent<Collider3D>();
                }
            }
        }
    }
}

// Created by Elijah Fabon

#include "game/systems/DoorPuzzleSystem.h"
#include "game/systems/DoorAnimationHelper.h"

#include "game/components/Components.h"

#include <unordered_map>

namespace game
{
    void DoorPuzzleSystem::Update(engine::Registry& registry, float deltaTime)
    {
        // First pass: build a lookup of switch ID -> pressed state
        std::unordered_map<std::string, bool> triggerStates;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Switch>(entity)) continue;
            auto& sw = registry.GetComponent<Switch>(entity);
            triggerStates[sw.Id] = sw.Pressed;
        }

        // Second pass: for each trigger-linked door, sync open state to its switch
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Door>(entity) ||
                !registry.HasComponent<Transform>(entity))
            {
                continue;
            }

            auto& door = registry.GetComponent<Door>(entity);

            // Only handle trigger-based doors (skip proximity doors)
            if (door.TriggerId.empty()) continue;

            auto& transform = registry.GetComponent<Transform>(entity);

            bool shouldOpen = false;
            auto it = triggerStates.find(door.TriggerId);
            if (it != triggerStates.end())
                shouldOpen = it->second;

            if (shouldOpen != door.Open)
            {
                door.Open = shouldOpen;
                door.TargetAngle = door.Open ? door.OpenAngle : door.CloseAngle;
            }

            DoorAnimationHelper::AnimateDoor(registry, entity, door, transform,
                                             door.TargetAngle, deltaTime);
        }
    }
}

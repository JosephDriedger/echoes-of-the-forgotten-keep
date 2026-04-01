#include "game/systems/DoorPuzzleSystem.h"

#include "game/components/Door.h"
#include "game/components/Switch.h"
#include "game/components/Transform.h"
#include "game/components/Collider.h"

#include <glm/glm.hpp>
#include <cmath>
#include <unordered_map>

namespace game
{
    void DoorPuzzleSystem::Update(engine::Registry& registry, float deltaTime)
    {
        // Collect switch/trigger states
        std::unordered_map<std::string, bool> triggerStates;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Switch>(entity)) continue;
            auto& sw = registry.GetComponent<Switch>(entity);
            triggerStates[sw.Id] = sw.Pressed;
        }

        // Update doors that have a TriggerId
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

            float diff = door.TargetAngle - door.CurrentAngle;
            if (std::abs(diff) > 0.01f)
            {
                float step = door.SwingSpeed * deltaTime;
                if (std::abs(diff) <= step)
                    door.CurrentAngle = door.TargetAngle;
                else
                    door.CurrentAngle += (diff > 0 ? step : -step);

                transform.RotationY = door.BaseRotationY + glm::radians(door.CurrentAngle);

                // Update the physical door collider
                if (door.ColliderEntity.IsValid() && registry.IsAlive(door.ColliderEntity) &&
                    registry.HasComponent<Transform>(door.ColliderEntity) &&
                    registry.HasComponent<Collider>(door.ColliderEntity))
                {
                    float totalAngle = door.BaseRotationY + glm::radians(door.CurrentAngle);
                    float halfPanel = door.PanelLength * 0.5f;

                    auto& colT = registry.GetComponent<Transform>(door.ColliderEntity);
                    colT.X = transform.X + std::cos(totalAngle) * halfPanel;
                    colT.Z = transform.Z - std::sin(totalAngle) * halfPanel;

                    auto& col = registry.GetComponent<Collider>(door.ColliderEntity);
                    float absS = std::abs(std::sin(totalAngle));
                    float absC = std::abs(std::cos(totalAngle));
                    col.Width = door.PanelLength * absC + door.PanelThickness * absS;
                    col.Depth = door.PanelLength * absS + door.PanelThickness * absC;
                }
            }
        }
    }
}

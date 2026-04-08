// Created by Elijah Fabon

#include "game/systems/DoorAnimationHelper.h"

#include "game/components/Components.h"

#include <glm/glm.hpp>
#include <cmath>

namespace game
{
    void DoorAnimationHelper::AnimateDoor(engine::Registry& registry,
                                          engine::Entity entity,
                                          Door& door, Transform& transform,
                                          float goalAngle, float deltaTime)
    {
        // Move CurrentAngle toward goalAngle at a fixed speed per second.
        // Skip if already close enough to avoid jitter.
        float diff = goalAngle - door.CurrentAngle;
        if (std::abs(diff) < 0.01f)
            return;

        float step = door.SwingSpeed * deltaTime;

        // Snap to goal when within one step to prevent overshooting
        if (std::abs(diff) <= step)
            door.CurrentAngle = goalAngle;
        else
            door.CurrentAngle += (diff > 0 ? step : -step);

        transform.RotationY = door.BaseRotationY + glm::radians(door.CurrentAngle);

        // Reposition the separate AABB collider entity so it tracks the
        // swinging door panel. The collider center sits at the panel midpoint,
        // and its width/depth are recomputed from the panel's rotated extents.
        if (door.ColliderEntity.IsValid() && registry.IsAlive(door.ColliderEntity) &&
            registry.HasComponent<Transform>(door.ColliderEntity) &&
            registry.HasComponent<Collider>(door.ColliderEntity))
        {
            float totalAngle = door.BaseRotationY + glm::radians(door.CurrentAngle);
            float halfPanel = door.PanelLength * 0.5f;

            auto& colT = registry.GetComponent<Transform>(door.ColliderEntity);
            colT.X = transform.X + std::cos(totalAngle) * halfPanel;
            colT.Z = transform.Z - std::sin(totalAngle) * halfPanel;

            // Axis-aligned bounding box of the rotated panel rectangle
            auto& col = registry.GetComponent<Collider>(door.ColliderEntity);
            float absS = std::abs(std::sin(totalAngle));
            float absC = std::abs(std::cos(totalAngle));
            col.Width = door.PanelLength * absC + door.PanelThickness * absS;
            col.Depth = door.PanelLength * absS + door.PanelThickness * absC;
        }
    }
}

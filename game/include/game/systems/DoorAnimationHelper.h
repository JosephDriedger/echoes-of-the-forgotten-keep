// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DOORANIMATIONHELPER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DOORANIMATIONHELPER_H

#include "engine/ecs/Registry.h"

namespace game
{
    struct Door;
    struct Transform;

    // Shared helper for animating a door toward its target angle and
    // updating its collider geometry. Used by both DoorSystem (proximity
    // doors) and DoorPuzzleSystem (trigger-based doors).
    namespace DoorAnimationHelper
    {
        // Animate door.CurrentAngle toward goalAngle at door.SwingSpeed.
        // Updates the door's Transform rotation and its collider entity.
        void AnimateDoor(engine::Registry& registry, engine::Entity entity,
                         Door& door, Transform& transform,
                         float goalAngle, float deltaTime);
    }
}

#endif

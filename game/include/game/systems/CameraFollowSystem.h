// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERAFOLLOWSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERAFOLLOWSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"
#include "engine/rendering/Camera.h"
#include "engine/input/Input.h"

namespace game
{
    /// Top-down camera that tracks a target entity with mouse-wheel zoom.
    /// The camera is positioned directly above the target, offset slightly
    /// along +Z proportional to the zoom level to create a mild angle.
    /// Zoom state is stored in CameraFollowState on the target entity.
    class CameraFollowSystem
    {
    public:
        static void Update(engine::Registry& registry, engine::Entity target,
                           engine::Camera& camera, const engine::Input& input);

    private:
        static constexpr float kZoomMin   = 8.0f;
        static constexpr float kZoomMax   = 60.0f;
        static constexpr float kZoomSpeed = 0.8f;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERAFOLLOWSYSTEM_H

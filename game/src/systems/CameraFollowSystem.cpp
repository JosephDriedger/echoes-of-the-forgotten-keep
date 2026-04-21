// Created by Elijah Fabon

#include "game/systems/CameraFollowSystem.h"

#include "game/components/Components.h"

namespace game
{
    void CameraFollowSystem::Update(engine::Registry& registry, engine::Entity target,
                                     engine::Camera& camera, const engine::Input& input)
    {
        if (!target.IsValid() || !registry.IsAlive(target))
            return;

        if (!registry.HasComponent<CameraFollowState>(target))
            registry.AddComponent(target, CameraFollowState{});

        auto& state = registry.GetComponent<CameraFollowState>(target);

        float scroll = input.GetMouseWheelY();
        if (scroll != 0.0f)
        {
            state.Zoom -= scroll * kZoomSpeed;
            if (state.Zoom < kZoomMin) state.Zoom = kZoomMin;
            if (state.Zoom > kZoomMax) state.Zoom = kZoomMax;
        }

        const auto& transform = registry.GetComponent<Transform>(target);
        float zOffset = state.Zoom * 0.4f;
        camera.SetPosition(transform.X, state.Zoom, transform.Z + zOffset);
        camera.SetTarget(transform.X, 0.0f, transform.Z);
    }
}

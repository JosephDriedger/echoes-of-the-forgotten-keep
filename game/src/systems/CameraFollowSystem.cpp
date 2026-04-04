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

        // Scroll wheel zoom
        float scroll = input.GetMouseWheelY();
        if (scroll != 0.0f)
        {
            m_Zoom -= scroll * kZoomSpeed;
            if (m_Zoom < kZoomMin) m_Zoom = kZoomMin;
            if (m_Zoom > kZoomMax) m_Zoom = kZoomMax;
        }

        const auto& transform = registry.GetComponent<Transform>(target);
        float zOffset = m_Zoom * 0.4f;
        camera.SetPosition(transform.X, m_Zoom, transform.Z + zOffset);
        camera.SetTarget(transform.X, 0.0f, transform.Z);
    }
}

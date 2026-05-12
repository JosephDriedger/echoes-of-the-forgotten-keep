//
// Created by adamd on 2026-05-09.
//

#include "game/systems/VisibilitySystem.h"

#include "game/components/Components.h"
#include "game/systems/RenderStats.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace game
{
    void VisibilitySystem::Update(
        engine::Registry& registry,
        engine::Camera& camera,
        engine::Frustum& frustum,
        RenderStats& renderStats,
        const bool disableCull)
    {
        renderStats.BeginFrame();

        frustum.Update(
            glm::make_mat4(camera.GetProjectionMatrix()) *
            glm::make_mat4(camera.GetViewMatrix()));

        for (const engine::Entity entity : registry.View<Transform, Render, Visibility>())
        {
            auto& visibility =
                registry.GetComponent<Visibility>(entity);

            visibility.IsVisible = true;

            if (disableCull)
            {
                renderStats.OnEntityRendered();
                continue;
            }

            const auto& transform =
                registry.GetComponent<Transform>(entity);

            const auto& render =
                registry.GetComponent<Render>(entity);

            if (!render.MeshPtr)
            {
                continue;
            }

            const auto& mesh = render.MeshPtr;

            glm::vec3 center =
                glm::vec3(transform.X,
                          transform.Y,
                          transform.Z)
                + mesh->GetBoundsCenter();

            const float radius =
                mesh->GetBoundsRadius() *
                std::max({transform.ScaleX,
                          transform.ScaleY,
                          transform.ScaleZ});

            visibility.IsVisible =
                frustum.IntersectsSphere(center, radius);

            if (visibility.IsVisible)
            {
                renderStats.OnEntityRendered();
            }
            else
            {
                renderStats.OnEntityCulled();
            }
        }
    }
}
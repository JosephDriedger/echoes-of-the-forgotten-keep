//
// Created by adamd on 2026-05-09.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_VISIBILITYSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_VISIBILITYSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/rendering/Camera.h"
#include "engine/Math/Frustum.h"

namespace game
{
    class RenderStats;

    class VisibilitySystem
    {
    public:
        static void Update(
            engine::Registry& registry,
            engine::Camera& camera,
            engine::Frustum& frustum,
            RenderStats& renderStats,
            bool disableCull);
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_VISIBILITYSYSTEM_H
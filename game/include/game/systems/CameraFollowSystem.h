#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERAFOLLOWSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERAFOLLOWSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"
#include "engine/rendering/Camera.h"
#include "engine/input/Input.h"

namespace game
{
    class CameraFollowSystem
    {
    public:
        void Update(engine::Registry& registry, engine::Entity target,
                    engine::Camera& camera, const engine::Input& input);

    private:
        float m_Zoom = 20.0f;
        static constexpr float kZoomMin = 8.0f;
        static constexpr float kZoomMax = 60.0f;
        static constexpr float kZoomSpeed = 0.8f;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERAFOLLOWSYSTEM_H

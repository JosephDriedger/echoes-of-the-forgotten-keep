#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_TRANSFORM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_TRANSFORM_H

#include <glm/glm.hpp>

namespace game
{
    struct Transform
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;

        float RotationY = 0.0f;

        float ScaleX = 1.0f;
        float ScaleY = 1.0f;
        float ScaleZ = 1.0f;

        glm::mat4 ModelMatrix = glm::mat4(1.0f);
        bool UseModelMatrix = false;

        Transform() = default;

        Transform(const float x, const float y, const float z = 0.0f)
            : X(x), Y(y), Z(z)
        {
        }
    };
}

#endif

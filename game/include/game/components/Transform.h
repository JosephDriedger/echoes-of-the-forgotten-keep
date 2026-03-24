//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_TRANSFORM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_TRANSFORM_H

namespace game
{
    struct Transform
    {
        float X;
        float Y;
        float Z;

        Transform()
            : X(0.0f), Y(0.0f), Z(0.0f)
        {
        }

        Transform(const float x, const float y, const float z = 0.0f)
            : X(x), Y(y), Z(z)
        {
        }
    };
}

#endif
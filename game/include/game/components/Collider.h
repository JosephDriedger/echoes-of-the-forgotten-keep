//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COLLIDER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COLLIDER_H

namespace game
{
    struct Collider
    {
        float Width;
        float Height;
        float Depth;
        bool IsTrigger;

        Collider()
            : Width(1.0f), Height(1.0f), Depth(1.0f), IsTrigger(false)
        {
        }

        Collider(const float width, const float height, const float depth, const bool isTrigger = false)
            : Width(width), Height(height), Depth(depth), IsTrigger(isTrigger)
        {
        }

        Collider(const float width, const float height)
            : Width(width), Height(height), Depth(1.0f), IsTrigger(false)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COLLIDER_H
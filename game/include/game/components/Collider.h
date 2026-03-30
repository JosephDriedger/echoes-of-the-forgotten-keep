#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COLLIDER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COLLIDER_H

namespace game
{
    struct Collider
    {
        float Width;
        float Height;
        float Depth;

        // Offset from entity position to collider center (world space)
        float OffsetX = 0.0f;
        float OffsetY = 0.0f;
        float OffsetZ = 0.0f;

        bool IsTrigger;
        bool IsStatic;

        Collider()
            : Width(1.0f), Height(1.0f), Depth(1.0f), IsTrigger(false), IsStatic(false)
        {
        }

        Collider(const float width, const float height, const float depth, const bool isTrigger = false)
            : Width(width), Height(height), Depth(depth), IsTrigger(isTrigger), IsStatic(false)
        {
        }

        Collider(const float width, const float height)
            : Width(width), Height(height), Depth(1.0f), IsTrigger(false), IsStatic(false)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COLLIDER_H

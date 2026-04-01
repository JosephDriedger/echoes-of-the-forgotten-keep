#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_LIFETIME_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_LIFETIME_H

namespace game
{
    struct Lifetime
    {
        float Duration = 5.0f;

        Lifetime() = default;
        explicit Lifetime(float duration) : Duration(duration) {}
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_LIFETIME_H

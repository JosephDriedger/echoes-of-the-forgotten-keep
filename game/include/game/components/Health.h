//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_HEALTH_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_HEALTH_H

namespace game
{
    struct Health
    {
        int Current;
        int Maximum;

        Health()
            : Current(100),
              Maximum(100)
        {
        }

        Health(const int current, const int maximum)
            : Current(current),
              Maximum(maximum)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_HEALTH_H
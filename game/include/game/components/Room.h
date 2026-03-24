//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_ROOM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_ROOM_H

namespace game
{
    struct Room
    {
        int Id;

        Room()
            : Id(0)
        {
        }

        explicit Room(const int id)
            : Id(id)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_ROOM_H
#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_SWITCH_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_SWITCH_H

#include <string>

namespace game
{
    struct Switch
    {
        std::string Id;
        bool Pressed = false;

        Switch() = default;
        explicit Switch(std::string id) : Id(std::move(id)) {}
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_SWITCH_H

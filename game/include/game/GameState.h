// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESTATE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESTATE_H

namespace game
{
    enum class GameState
    {
        MainMenu,
        Playing,
        Paused,
        GameOver,
        Quitting
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESTATE_H
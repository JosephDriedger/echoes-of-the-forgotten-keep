// Created by Joey Driedger
/// @file GameState.h
/// @brief High-level game states used to track the overall application mode.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESTATE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESTATE_H

namespace game
{
    /// Represents the logical state of the game, independent of scene stack.
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
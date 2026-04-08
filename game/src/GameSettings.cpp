// Created by Adam Van Woerden

#include "game/GameSettings.h"

namespace game
{
    /// Meyer's singleton -- the static local is created on first call and
    /// persists for the lifetime of the process.
    GameSettings& GameSettings::Instance()
    {
        static GameSettings instance;
        return instance;
    }
}

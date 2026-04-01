#include "game/GameSettings.h"

namespace game
{
    GameSettings& GameSettings::Instance()
    {
        static GameSettings instance;
        return instance;
    }
}

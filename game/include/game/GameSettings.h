#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESETTINGS_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESETTINGS_H

namespace game
{
    struct GameSettings
    {
        float MusicVolume = 0.75f;
        float VFXVolume = 0.75f;

        static GameSettings& Instance();
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESETTINGS_H

// Created by Adam Van Woerden
/// @file GameSettings.h
/// @brief Global game settings singleton (audio volumes, etc.).
///
/// GameSettings::Instance() provides a single shared instance that
/// scenes read and write to. The SettingsScene modifies values via
/// UISlider, and the audio system (when integrated) will read them.
///
/// SettingsReturnScene tracks which scene the Settings screen should
/// navigate back to, since Settings can be reached from both the
/// main menu and the pause menu.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESETTINGS_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESETTINGS_H

#include <string>

namespace game
{
    struct GameSettings
    {
        float MusicVolume = 0.75f; ///< 0.0 (mute) to 1.0 (full)
        float VFXVolume = 0.75f;   ///< 0.0 (mute) to 1.0 (full)

        /// Which scene to return to when leaving the settings screen.
        /// Set by GameApp before transitioning to SettingsScene.
        std::string SettingsReturnScene = "MainMenuScene";

        static GameSettings& Instance();
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMESETTINGS_H

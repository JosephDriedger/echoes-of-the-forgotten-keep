//
// Created by adamd on 2026-04-03.
//

#ifndef ASSIGNMENT1_AUDIOMANAGER_H
#define ASSIGNMENT1_AUDIOMANAGER_H

#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"

namespace engine {
    class AudioManager {
        MIX_Mixer *mixer = nullptr;
        MIX_Track *musicTrack = nullptr;
        static MIX_Track *sfxTrack;
        static std::unordered_map<std::string, MIX_Audio*> audio;

    public:
        AudioManager();

        void loadAudio(const std::string& name, const char* path) const;

        void playMusic(const std::string& name) const;
        void stopMusic() const;

        static void playSfx(const std::string& name);

        void SetMusicVolume(float volume) const;
        void SetSFXVolume(float volume);
    };
}
#endif //ASSIGNMENT1_AUDIOMANAGER_H
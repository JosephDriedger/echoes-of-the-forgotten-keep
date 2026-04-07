//
// Created by adamd on 2026-04-03.
//

#ifndef ASSIGNMENT1_AUDIOMANAGER_H
#define ASSIGNMENT1_AUDIOMANAGER_H

#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"

namespace engine {
    // Manages audio playback via SDL3_mixer.
    // Uses two tracks: one for looping background music and one for one-shot SFX.
    // Audio assets are stored in a static map so SFX can be played from anywhere
    // without requiring an instance reference (see playSfx).
    class AudioManager {
        MIX_Mixer *mixer = nullptr;
        MIX_Track *musicTrack = nullptr;       // Dedicated track for background music.
        static MIX_Track *sfxTrack;            // Shared track for sound effects.
        static std::unordered_map<std::string, MIX_Audio*> audio; // Name-to-audio asset cache.

    public:
        // Initializes SDL3_mixer, creates the mixer device and both tracks.
        AudioManager();

        // Loads an audio file and registers it under the given name. No-op if already loaded.
        void loadAudio(const std::string& name, const char* path) const;

        // Begins looping playback of a named audio asset on the music track.
        void playMusic(const std::string& name) const;
        void stopMusic() const;

        // Plays a one-shot sound effect. Static so it can be called from systems
        // that lack a direct AudioManager reference (e.g. via AudioEventQueue).
        static void playSfx(const std::string& name);

        void SetMusicVolume(float volume) const;
        void SetSFXVolume(float volume);
    };
}
#endif //ASSIGNMENT1_AUDIOMANAGER_H
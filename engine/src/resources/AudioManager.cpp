//
// Created by adamd on 2026-04-03.
//

#include "engine/resources/AudioManager.h"

#include <iostream>
#include <ostream>

namespace engine {
    MIX_Track* AudioManager::sfxTrack;
    std::unordered_map<std::string, MIX_Audio*> AudioManager::audio;

    AudioManager::AudioManager() {

        if (MIX_Init() == 0) {
            std::cout << "MIX_Init() failed." << std::endl;
            return;
        }

        mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!mixer) {
            std::cout << "MIX_CreateMixerDevice() failed." << std::endl;
            return;
        }

        musicTrack = MIX_CreateTrack(mixer);
        sfxTrack = MIX_CreateTrack(mixer);
        MIX_SetTrackGain(musicTrack, 0.75f);
    }

    void AudioManager::loadAudio(const std::string &name, const char *path) const {

        if (audio.contains(name)) {
            return;
        }

        auto audioPtr = MIX_LoadAudio(mixer, path, true);
        if (!audioPtr) {
            std::cout << "MIX_LoadAudio() failed." << std::endl;
            return;
        }

        audio.emplace(name, audioPtr);
    }

    void AudioManager::playMusic(const std::string& name) const {

        if (MIX_SetTrackAudio(musicTrack, audio[name]) == 0) {
            std::cout << "MIX_SetTrackAudio() failed." << std::endl;
            return;
        }

        MIX_PlayTrack(musicTrack, -1); // means loop endless
        std::cout << "Playing Music: " << name << std::endl;
    }

    void AudioManager::stopMusic() const {
        MIX_StopTrack(musicTrack, 0);
    }

    void AudioManager::playSfx(const std::string& name) {

        if (MIX_SetTrackAudio(sfxTrack, audio[name]) == 0) {
            std::cout << "MIX_SetTrackAudio() failed." << std::endl;
            return;
        }

        MIX_PlayTrack(sfxTrack, 0);
        std::cout << "Playing sfx: " << name << std::endl;
    }

    void AudioManager::SetMusicVolume(float volume) const {
        MIX_SetTrackGain(musicTrack, volume);
    }

    void AudioManager::SetSFXVolume(float volume)
    {
        MIX_SetTrackGain(sfxTrack, volume);
    }
}
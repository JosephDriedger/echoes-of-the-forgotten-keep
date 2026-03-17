//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DCLIPS_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DCLIPS_H
#include "AnimationBone.h"

struct Animation3DClip {
    std::string name;
    float duration = 0.0f;
    float ticksPerSecond = 25.0f;
    std::unordered_map<std::string, AnimationBone> bones;
};


#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DCLIPS_H
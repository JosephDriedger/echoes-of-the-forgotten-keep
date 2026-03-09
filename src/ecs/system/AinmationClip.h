//
// Created by adamd on 2026-02-18.
//

#ifndef ASSIGNMENT1_AINMATIONCLIP_H
#define ASSIGNMENT1_AINMATIONCLIP_H
#include <vector>
#include <SDL3/SDL_rect.h>

struct AnimationClip {
    std::vector<SDL_FRect> frameIndices;
};

#endif //ASSIGNMENT1_AINMATIONCLIP_H
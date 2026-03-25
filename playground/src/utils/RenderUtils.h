//
// Created by scept on 2026-03-14.
//

#ifndef INC_8051TUTORIAL_RENDERUTILS_H
#define INC_8051TUTORIAL_RENDERUTILS_H
#include "SDL3/SDL_rect.h"

class RenderUtils {
    public:
    // scaling from center point
    static SDL_FRect getScaledDest(const SDL_FRect& dest, float scale) {
        return SDL_FRect(
            dest.x + dest.w * (1- scale) / 2,
            dest.y + dest.h * (1- scale) / 2,
            dest.w * scale,
            dest.h * scale
        );
    }
};

#endif //INC_8051TUTORIAL_RENDERUTILS_H
//
// Created by adamd on 2026-02-04.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISION_H

#include <SDL3/SDL_rect.h>

#include "Component.h"

class Collision {
public:
    // axis-aligned bounding box
    static bool AABB(const SDL_FRect& rectA, const SDL_FRect& rectB);
    static bool AABB(const Collider& colA, const Collider& colB);
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISION_H
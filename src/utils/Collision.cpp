//
// Created by adamd on 2026-02-04.
//

#include "Collision.h"

// axis-aligned bounding box
// passing in rects
bool Collision::AABB(const SDL_FRect& rectA, const SDL_FRect& rectB) {

    // 1. is the right edge of rectA >= to the left edge of rectB
    // 2. is the left edge of rectB >= to the right edge of rectA
    // 3. is the bottom edge of rectA >= to the top edge of rectB
    // 4. is the top edge of rectB >= to the bottom edge of rectA
    if (
        rectA.x + rectA.w >= rectB.x &&
        rectB.x + rectB.w >= rectA.x &&
        rectA.y + rectA.h >= rectB.y &&
        rectB.y + rectB.h >= rectA.y) {
        return true;
    }
    return false;
}

// passing in the colliders itself
bool Collision::AABB(const Collider& colA, const Collider& colB) {
    if (AABB(colA.rect, colB.rect)) {
        return true;
    }
    return false;
}

glm::vec3 getMin(const Transform3D& t, const Collider3D& c) {
    return t.position + c.offset - (c.size * 0.5f) * t.scale;
}

glm::vec3 getMax(const Transform3D& t, const Collider3D& c) {
    return t.position + c.offset + (c.size * 0.5f) * t.scale;
}

bool Collision::AABB3D(const Transform3D& tA, const Collider3D& cA,
                       const Transform3D& tB, const Collider3D& cB) {

    if (!cA.enabled || !cB.enabled) return false;

    glm::vec3 minA = getMin(tA, cA);
    glm::vec3 maxA = getMax(tA, cA);

    glm::vec3 minB = getMin(tB, cB);
    glm::vec3 maxB = getMax(tB, cB);

    if (
        maxA.x >= minB.x && minA.x <= maxB.x &&
        maxA.y >= minB.y && minA.y <= maxB.y &&
        maxA.z >= minB.z && minA.z <= maxB.z
    ) {
        return true;
    }

    return false;
}

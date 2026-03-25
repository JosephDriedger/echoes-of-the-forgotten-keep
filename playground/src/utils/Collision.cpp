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

bool Collision::AABB3D(const Transform3D& transformA, const Collider3D& colliderA,
                       const Transform3D& transformB, const Collider3D& colliderB) {

    // return transformA.position.x < transformB.position.x + colliderB.Width &&
    //            transformA.position.x + colliderA.Width > transformB.position.x &&
    //            transformA.position.y < transformB.position.y + colliderB.Height &&
    //            transformA.position.y + colliderA.Height > transformB.position.y &&
    //            transformA.position.z < transformB.position.z + colliderB.Depth &&
    //            transformA.position.z + colliderA.Depth > transformB.position.z;

    return
    abs(transformA.position.x - transformB.position.x) < (colliderA.Width * 0.5f + colliderB.Width * 0.5f) &&
    abs(transformA.position.y - transformB.position.y) < (colliderA.Height * 0.5f + colliderB.Height * 0.5f) &&
    abs(transformA.position.z - transformB.position.z) < (colliderA.Depth * 0.5f + colliderB.Depth * 0.5f);
}

// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H

#include "engine/ecs/Registry.h"

#include "game/components/Components.h"

namespace game
{
    /// Brute-force AABB collision detection and resolution for all entity pairs.
    /// Skips static-vs-static pairs and trigger colliders (overlap-only, no push-out).
    /// Resolution pushes entities apart along the axis of smallest overlap;
    /// static objects never move -- only the dynamic partner is displaced.
    class CollisionSystem
    {
    public:
        void Update(engine::Registry& registry) const;

    private:
        /// Returns true if the two AABBs (with collider offsets) overlap on all three axes.
        static bool IsColliding(const Transform& transformA,
                                const Collider& colliderA,
                                const Transform& transformB,
                                const Collider& colliderB);

        /// Separates two overlapping AABBs along the axis of minimum penetration.
        static void ResolveCollision(Transform& transformA,
                                     const Collider& colliderA,
                                     Transform& transformB,
                                     const Collider& colliderB);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H

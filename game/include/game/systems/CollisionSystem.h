#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H

#include "engine/ecs/Registry.h"

#include "game/components/Collider.h"
#include "game/components/Transform.h"

namespace game
{
    class CollisionSystem
    {
    public:
        void Update(engine::Registry& registry) const;

    private:
        static bool IsColliding(const Transform& transformA,
                                const Collider& colliderA,
                                const Transform& transformB,
                                const Collider& colliderB);

        static void ResolveCollision(Transform& transformA,
                                     const Collider& colliderA,
                                     Transform& transformB,
                                     const Collider& colliderB);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H

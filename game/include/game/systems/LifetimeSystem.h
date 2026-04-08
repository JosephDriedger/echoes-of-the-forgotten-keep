// Created by Elijah Fabon

// LifetimeSystem -- Counts down the Duration on entities with a Lifetime
// component and destroys them when it reaches zero. Used for temporary
// effects like projectiles, particles, and hit-flash overlays.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_LIFETIMESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_LIFETIMESYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class LifetimeSystem
    {
    public:
        static void Update(engine::Registry& registry, float deltaTime);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_LIFETIMESYSTEM_H

// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGESYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    /// Processes pending hits queued by AttackHitboxSystem.
    /// Subtracts damage from Health, applies directional knockback to enemies,
    /// triggers the hit-react animation, grants a brief invincibility window,
    /// and marks entities with zero HP as dead.
    class DamageSystem
    {
    public:
        void Update(engine::Registry& registry);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGESYSTEM_H

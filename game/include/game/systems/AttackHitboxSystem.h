// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    /// Spawns, positions, and manages AABB hitbox entities for melee attacks.
    /// Each attacker gets one hitbox placed in front of them while IsAttacking is true.
    /// Per-swing hit tracking and hitbox lifetime are stored in AttackStateComponent.
    class AttackHitboxSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

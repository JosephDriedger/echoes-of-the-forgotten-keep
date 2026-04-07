// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    /// Counts down the HitTimer on every CombatState each frame.
    /// While HitTimer > 0 the entity has invincibility frames (i-frames)
    /// and cannot receive new hits from AttackHitboxSystem.
    class HitTimerSystem
    {
    public:
        void Update(engine::Registry& registry, float deltaTime);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATSYSTEM_H

// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

#include <unordered_set>

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    /// Spawns, positions, and manages AABB hitbox entities for melee attacks.
    /// Each attacker gets one hitbox placed in front of them while IsAttacking is true.
    /// Tracks which targets were already hit per swing to prevent multi-hit, and
    /// resets on combo-index changes so each swing in a combo can hit independently.
    class AttackHitboxSystem
    {
    public:
        void Update(engine::Registry& registry);

    private:
        // Maps attacker entity ID -> its current hitbox entity
        std::unordered_map<uint32_t, engine::Entity> m_ActiveHitboxes;
        // Maps attacker entity ID -> set of target IDs already hit this swing
        std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_HitThisSwing;
        // Tracks the last combo index per attacker to detect new swings
        std::unordered_map<uint32_t, int> m_LastComboIndex;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

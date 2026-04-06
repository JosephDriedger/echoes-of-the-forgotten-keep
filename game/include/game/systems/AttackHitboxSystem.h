// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

#include <unordered_set>

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    class AttackHitboxSystem
    {
    public:
        void Update(engine::Registry& registry);

    private:
        // engine::Entity m_AttackHitbox{0};
        // One active hitbox entity per attacker
        std::unordered_map<uint32_t, engine::Entity> m_ActiveHitboxes;
        // Entities already hit this swing, per attacker
        std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_HitThisSwing;
        std::unordered_map<uint32_t, int> m_LastComboIndex;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

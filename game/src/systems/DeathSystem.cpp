// Created by Elijah Fabon

#include "game/systems/DeathSystem.h"

#include "game/components/Components.h"

namespace game
{
    void DeathSystem::Update(engine::Registry& registry)
    {
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Health>(entity))
                continue;

            const auto& health = registry.GetComponent<Health>(entity);
            if (health.Current > 0)
                continue;

            // Mark combat state as dead
            if (registry.HasComponent<CombatState>(entity))
            {
                auto& combat = registry.GetComponent<CombatState>(entity);
                combat.IsDead = true;
                combat.IsAttacking = false;
            }

            // Set death animation
            if (registry.HasComponent<AnimationState>(entity))
            {
                auto& anim = registry.GetComponent<AnimationState>(entity);
                anim.IsMoving = false;
            }

            // Add corpse timer so the entity gets cleaned up
            if (!registry.HasComponent<Lifetime>(entity))
            {
                registry.AddComponent(entity, Lifetime(3.0f));
            }
        }
    }
}

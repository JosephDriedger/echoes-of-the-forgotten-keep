// Created by Elijah Fabon

#include "game/systems/DeathSystem.h"

#include "game/components/Components.h"

namespace game
{
    void DeathSystem::Update(engine::Registry& registry, engine::AudioEventQueue &audioEventQueue)
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

            // Disable physics (make trigger) and play death sound once.
            // The IsTrigger check prevents replaying the sound on subsequent frames.
            if (registry.HasComponent<AnimationState>(entity))
            {
                auto& anim = registry.GetComponent<AnimationState>(entity);
                if (registry.HasComponent<Collider>(entity)) {
                    auto& col = registry.GetComponent<Collider>(entity);
                    if (col.IsTrigger == false) {
                        audioEventQueue.push(std::make_unique<engine::AudioEvent>("death"));
                    }
                    col.IsTrigger = true; // corpse no longer blocks movement
                }
                anim.IsMoving = false;
            }

            // Attach a Lifetime component so the entity is destroyed after 3 seconds
            if (!registry.HasComponent<Lifetime>(entity))
            {
                registry.AddComponent(entity, Lifetime(3.0f));
            }
        }
    }
}

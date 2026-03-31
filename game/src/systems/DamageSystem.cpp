//
// Created by scept on 2026-03-09.
//

#include "game/systems/DamageSystem.h"
#include "game/components/Health.h"
#include "game/components/DamageEvent.h"
#include "game/components/AnimationState.h"
#include <iostream>

namespace game
{
    void DamageSystem::Update(engine::Registry& registry)
    {
        for (auto entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<DamageEvent>(entity))
                continue;

            auto& dmg = registry.GetComponent<DamageEvent>(entity);

            if (!registry.IsAlive(*dmg.target))
                continue;

            if (registry.HasComponent<Health>(*dmg.target))
            {
                auto& health = registry.GetComponent<Health>(*dmg.target);
                health.Current -= dmg.amount;

                std::cout << "Damage: " << dmg.amount
                          << " HP left: " << health.Current << "\n";

                if (health.Current <= 0 &&
                    registry.HasComponent<AnimationState>(*dmg.target))
                {
                    auto& anim = registry.GetComponent<AnimationState>(*dmg.target);
                    anim.isDead = true;
                }
            }

            registry.DestroyEntity(entity);
        }
    }
}
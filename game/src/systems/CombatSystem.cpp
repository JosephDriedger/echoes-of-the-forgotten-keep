#include "game/systems/CombatSystem.h"

#include "game/components/CombatState.h"
#include "game/components/EnemyAI.h"
#include "game/components/Health.h"
#include "game/components/Transform.h"
#include "game/components/Collider.h"
#include "game/components/Player.h"

#include <cmath>
#include <iostream>

namespace game
{
    void CombatSystem::Update(engine::Registry& registry, float deltaTime)
    {
        // Find the player entity
        engine::Entity playerEntity(0);
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (registry.HasComponent<Player>(entity) &&
                registry.HasComponent<Transform>(entity))
            {
                playerEntity = entity;
                break;
            }
        }

        if (!playerEntity.IsValid() || !registry.IsAlive(playerEntity))
            return;

        const auto& playerTransform = registry.GetComponent<Transform>(playerEntity);

        // Check enemy attacks against the player
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<EnemyAI>(entity) ||
                !registry.HasComponent<Transform>(entity) ||
                !registry.HasComponent<CombatState>(entity))
                continue;

            auto& combat = registry.GetComponent<CombatState>(entity);
            const auto& enemyTransform = registry.GetComponent<Transform>(entity);

            // Check if enemy is in attack range
            float dx = playerTransform.X - enemyTransform.X;
            float dz = playerTransform.Z - enemyTransform.Z;
            float dist = std::sqrt(dx * dx + dz * dz);

            if (combat.IsAttacking && dist < 2.0f)
            {
                // Apply damage to player
                if (registry.HasComponent<Health>(playerEntity))
                {
                    auto& health = registry.GetComponent<Health>(playerEntity);
                    health.Current -= 1;

                    std::cout << "Enemy attacks player! HP: " << health.Current << std::endl;

                    if (health.Current <= 0)
                    {
                        auto& playerCombat = registry.GetComponent<CombatState>(playerEntity);
                        playerCombat.IsDead = true;
                    }
                }
            }
        }
    }
}

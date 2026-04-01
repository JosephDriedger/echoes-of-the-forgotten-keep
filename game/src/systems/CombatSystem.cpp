#include "game/systems/CombatSystem.h"

#include "game/components/CombatState.h"
#include "game/components/EnemyAI.h"
#include "game/components/Health.h"
#include "game/components/Transform.h"
#include "game/components/Player.h"

#include <cmath>

namespace game
{
    void CombatSystem::Update(engine::Registry& registry, float deltaTime)
    {
        // Find the player
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

        // Check each enemy's attack against player
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<EnemyAI>(entity) ||
                !registry.HasComponent<Transform>(entity) ||
                !registry.HasComponent<CombatState>(entity))
            {
                continue;
            }

            auto& combat = registry.GetComponent<CombatState>(entity);
            const auto& enemyTransform = registry.GetComponent<Transform>(entity);

            if (combat.IsAttacking)
            {
                float dx = playerTransform.X - enemyTransform.X;
                float dz = playerTransform.Z - enemyTransform.Z;
                float dist = std::sqrt(dx * dx + dz * dz);

                if (dist < 2.0f && registry.HasComponent<Health>(playerEntity))
                {
                    auto& health = registry.GetComponent<Health>(playerEntity);
                    health.Current -= 1;
                    if (health.Current <= 0)
                    {
                        if (registry.HasComponent<CombatState>(playerEntity))
                        {
                            auto& playerCombat = registry.GetComponent<CombatState>(playerEntity);
                            playerCombat.IsDead = true;
                        }
                    }
                }
                combat.IsAttacking = false;
            }
        }
    }
}

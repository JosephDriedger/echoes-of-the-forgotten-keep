#include "game/systems/EnemyAISystem.h"

#include "game/components/EnemyAI.h"
#include "game/components/Transform.h"
#include "game/components/Player.h"
#include "game/components/Health.h"
#include "game/components/CombatState.h"

#include <cmath>
#include <iostream>

namespace game
{
    float EnemyAISystem::Distance(float ax, float az, float bx, float bz)
    {
        float dx = ax - bx;
        float dz = az - bz;
        return std::sqrt(dx * dx + dz * dz);
    }

    void EnemyAISystem::Update(engine::Registry& registry, float deltaTime)
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

        // Update all enemies
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<EnemyAI>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            auto& ai = registry.GetComponent<EnemyAI>(entity);
            const auto& enemyTransform = registry.GetComponent<Transform>(entity);

            // Skip dead enemies
            if (registry.HasComponent<Health>(entity))
            {
                const auto& health = registry.GetComponent<Health>(entity);
                if (health.Current <= 0)
                    continue;
            }

            ai.StateTimer += deltaTime;

            switch (ai.State)
            {
                case AIState::Idle:
                    UpdateIdle(registry, entity, deltaTime);
                    break;
                case AIState::Chase:
                    if (playerEntity.IsValid())
                        UpdateChase(registry, entity, playerEntity, deltaTime);
                    else
                        ai.State = AIState::Idle;
                    break;
                case AIState::Attack:
                    if (playerEntity.IsValid())
                        UpdateAttack(registry, entity, playerEntity, deltaTime);
                    else
                        ai.State = AIState::Idle;
                    break;
            }

            // Global detection check (can interrupt idle)
            if (playerEntity.IsValid() && registry.IsAlive(playerEntity) &&
                registry.HasComponent<Transform>(playerEntity))
            {
                const auto& playerTransform = registry.GetComponent<Transform>(playerEntity);
                float dist = Distance(enemyTransform.X, enemyTransform.Z,
                                       playerTransform.X, playerTransform.Z);

                if (ai.LoseTargetTimer > 0)
                {
                    ai.LoseTargetTimer -= deltaTime;
                }
                else if (dist < ai.VisionRange && ai.State == AIState::Idle)
                {
                    ai.State = AIState::Chase;
                    ai.StateTimer = 0;
                }
            }
        }
    }

    void EnemyAISystem::UpdateIdle(engine::Registry& registry, engine::Entity enemy, float dt)
    {
        (void)dt;
        // Enemy stays still while idle
    }

    void EnemyAISystem::UpdateChase(engine::Registry& registry, engine::Entity enemy,
                                     engine::Entity player, float dt)
    {
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        auto& enemyTransform = registry.GetComponent<Transform>(enemy);
        const auto& playerTransform = registry.GetComponent<Transform>(player);

        float dist = Distance(enemyTransform.X, enemyTransform.Z,
                               playerTransform.X, playerTransform.Z);

        // Gave up chasing
        if (ai.StateTimer > ai.MaxChaseTime)
        {
            ai.State = AIState::Idle;
            ai.StateTimer = 0;
            ai.LoseTargetTimer = ai.LoseTargetCooldown;
            return;
        }

        // Close enough to attack
        if (dist <= ai.AttackRange)
        {
            ai.State = AIState::Attack;
            ai.StateTimer = 0;
            return;
        }

        // Lost sight
        if (dist > ai.VisionRange * 1.5f)
        {
            ai.State = AIState::Idle;
            ai.StateTimer = 0;
            return;
        }

        // Move toward player
        float dx = playerTransform.X - enemyTransform.X;
        float dz = playerTransform.Z - enemyTransform.Z;

        if (dist > 0.001f)
        {
            float nx = dx / dist;
            float nz = dz / dist;

            enemyTransform.X += nx * ai.MoveSpeed * dt;
            enemyTransform.Z += nz * ai.MoveSpeed * dt;

            // Face the player
            enemyTransform.RotationY = std::atan2(nx, nz);
        }
    }

    void EnemyAISystem::UpdateAttack(engine::Registry& registry, engine::Entity enemy,
                                      engine::Entity player, float dt)
    {
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        const auto& enemyTransform = registry.GetComponent<Transform>(enemy);
        const auto& playerTransform = registry.GetComponent<Transform>(player);

        float dist = Distance(enemyTransform.X, enemyTransform.Z,
                               playerTransform.X, playerTransform.Z);

        // Attack cooldown
        ai.AttackTimer += dt;
        if (ai.AttackTimer >= ai.AttackCooldown)
        {
            ai.AttackTimer = 0;

            std::cout << "Enemy attacks player!" << std::endl;

            if (registry.HasComponent<CombatState>(enemy))
            {
                auto& combat = registry.GetComponent<CombatState>(enemy);
                combat.IsAttacking = true;
            }
        }

        // Player moved away - chase again
        float attackExitRange = ai.AttackRange + 1.0f;
        if (dist > attackExitRange)
        {
            ai.State = AIState::Chase;
            ai.StateTimer = 0;
        }
    }
}

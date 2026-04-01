#include "game/systems/EnemyAISystem.h"

#include "game/components/EnemyAI.h"
#include "game/components/Transform.h"
#include "game/components/CombatState.h"
#include "game/components/Player.h"
#include "game/components/Health.h"
#include "game/components/AnimationState.h"

#include <cmath>

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
            if (registry.HasComponent<Player>(entity) && registry.HasComponent<Transform>(entity))
            {
                playerEntity = entity;
                break;
            }
        }

        // Update each enemy
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<EnemyAI>(entity) ||
                !registry.HasComponent<Transform>(entity))
            {
                continue;
            }

            auto& ai = registry.GetComponent<EnemyAI>(entity);

            // Skip dead enemies
            if (registry.HasComponent<Health>(entity))
            {
                auto& health = registry.GetComponent<Health>(entity);
                if (health.Current <= 0) continue;
            }

            ai.StateTimer += deltaTime;

            switch (ai.State)
            {
                case AIState::Idle:
                    UpdateIdle(registry, entity, deltaTime);
                    // Check if player is in vision range to start chasing
                    if (playerEntity.IsValid() && registry.IsAlive(playerEntity))
                    {
                        const auto& et = registry.GetComponent<Transform>(entity);
                        const auto& pt = registry.GetComponent<Transform>(playerEntity);
                        float dist = Distance(et.X, et.Z, pt.X, pt.Z);
                        if (dist < ai.VisionRange)
                        {
                            ai.State = AIState::Chase;
                            ai.StateTimer = 0.0f;
                        }
                    }
                    break;

                case AIState::Chase:
                    if (playerEntity.IsValid() && registry.IsAlive(playerEntity))
                        UpdateChase(registry, entity, playerEntity, deltaTime);
                    else
                    {
                        ai.State = AIState::Idle;
                        ai.StateTimer = 0.0f;
                    }
                    break;

                case AIState::Attack:
                    if (playerEntity.IsValid() && registry.IsAlive(playerEntity))
                        UpdateAttack(registry, entity, playerEntity, deltaTime);
                    else
                    {
                        ai.State = AIState::Idle;
                        ai.StateTimer = 0.0f;
                    }
                    break;
            }
        }
    }

    void EnemyAISystem::UpdateIdle(engine::Registry& registry, engine::Entity enemy, float dt)
    {
        // Set animation to idle
        if (registry.HasComponent<AnimationState>(enemy))
        {
            auto& anim = registry.GetComponent<AnimationState>(enemy);
            anim.IsMoving = false;
        }
    }

    void EnemyAISystem::UpdateChase(engine::Registry& registry, engine::Entity enemy, engine::Entity player, float dt)
    {
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        auto& et = registry.GetComponent<Transform>(enemy);
        const auto& pt = registry.GetComponent<Transform>(player);

        float dist = Distance(et.X, et.Z, pt.X, pt.Z);

        // If close enough, switch to attack
        if (dist < ai.AttackRange)
        {
            ai.State = AIState::Attack;
            ai.StateTimer = 0.0f;
            return;
        }

        // If chased too long, lose interest
        if (ai.StateTimer > ai.MaxChaseTime)
        {
            ai.State = AIState::Idle;
            ai.StateTimer = 0.0f;
            ai.LoseTargetTimer = ai.LoseTargetCooldown;
            return;
        }

        // If player out of extended range, stop chasing
        if (dist > ai.VisionRange * 1.5f)
        {
            ai.State = AIState::Idle;
            ai.StateTimer = 0.0f;
            return;
        }

        // Move toward player
        float dx = pt.X - et.X;
        float dz = pt.Z - et.Z;
        if (dist > 0.01f)
        {
            et.X += (dx / dist) * ai.MoveSpeed * dt;
            et.Z += (dz / dist) * ai.MoveSpeed * dt;

            // Face toward player
            et.RotationY = std::atan2(dx, dz);
        }

        // Set animation to walking
        if (registry.HasComponent<AnimationState>(enemy))
        {
            auto& anim = registry.GetComponent<AnimationState>(enemy);
            anim.IsMoving = true;
        }
    }

    void EnemyAISystem::UpdateAttack(engine::Registry& registry, engine::Entity enemy, engine::Entity player, float dt)
    {
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        auto& et = registry.GetComponent<Transform>(enemy);
        const auto& pt = registry.GetComponent<Transform>(player);

        float dist = Distance(et.X, et.Z, pt.X, pt.Z);

        // Face the player
        float dx = pt.X - et.X;
        float dz = pt.Z - et.Z;
        if (dist > 0.01f)
            et.RotationY = std::atan2(dx, dz);

        // Stop moving while attacking
        if (registry.HasComponent<AnimationState>(enemy))
        {
            auto& anim = registry.GetComponent<AnimationState>(enemy);
            anim.IsMoving = false;
        }

        // Attack cooldown
        ai.AttackTimer += dt;
        if (ai.AttackTimer >= ai.AttackCooldown)
        {
            ai.AttackTimer = 0.0f;

            // Deal damage to player
            if (registry.HasComponent<CombatState>(enemy))
            {
                auto& combat = registry.GetComponent<CombatState>(enemy);
                combat.IsAttacking = true;
            }

            if (registry.HasComponent<Health>(player))
            {
                auto& health = registry.GetComponent<Health>(player);
                health.Current -= 1;
                if (health.Current <= 0)
                {
                    if (registry.HasComponent<CombatState>(player))
                    {
                        auto& playerCombat = registry.GetComponent<CombatState>(player);
                        playerCombat.IsDead = true;
                    }
                }
            }
        }

        // If player moved away, chase again
        if (dist > ai.AttackRange + 1.0f)
        {
            ai.State = AIState::Chase;
            ai.StateTimer = 0.0f;
        }
    }
}

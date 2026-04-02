// Created by Elijah Fabon

#include "game/systems/EnemyAISystem.h"

#include "game/components/Components.h"

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

            // Skip dead enemies — add a corpse timer so they get cleaned up
            if (registry.HasComponent<Health>(entity))
            {
                auto& health = registry.GetComponent<Health>(entity);
                if (health.Current <= 0)
                {
                    if (!registry.HasComponent<Lifetime>(entity))
                    {
                        registry.AddComponent(entity, Lifetime(3.0f));

                        if (registry.HasComponent<AnimationState>(entity))
                        {
                            auto& anim = registry.GetComponent<AnimationState>(entity);
                            anim.CurrentState = AnimState::Death;
                            anim.IsMoving = false;
                        }
                    }
                    continue;
                }
            }

            // Decrease lose-target cooldown
            if (ai.LoseTargetTimer > 0.0f)
                ai.LoseTargetTimer -= deltaTime;

            ai.StateTimer += deltaTime;

            // Global detection check — can interrupt Idle/Patrol
            if (playerEntity.IsValid() && registry.IsAlive(playerEntity) &&
                ai.LoseTargetTimer <= 0.0f &&
                (ai.State == AIState::Idle || ai.State == AIState::Patrol))
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

            switch (ai.State)
            {
                case AIState::Idle:
                    UpdateIdle(registry, entity, deltaTime);
                    break;

                case AIState::Patrol:
                    UpdatePatrol(registry, entity, deltaTime);
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
        if (registry.HasComponent<AnimationState>(enemy))
        {
            auto& anim = registry.GetComponent<AnimationState>(enemy);
            anim.IsMoving = false;
        }

        // After idle timer, start patrolling if patrol is set up
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        if (ai.HasPatrol && ai.StateTimer > ai.IdleBeforePatrol)
        {
            ai.State = AIState::Patrol;
            ai.StateTimer = 0.0f;
        }
    }

    void EnemyAISystem::UpdatePatrol(engine::Registry& registry, engine::Entity enemy, float dt)
    {
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        auto& et = registry.GetComponent<Transform>(enemy);

        float targetX = ai.MovingToB ? ai.PatrolBX : ai.PatrolAX;
        float targetZ = ai.MovingToB ? ai.PatrolBZ : ai.PatrolAZ;

        float dist = Distance(et.X, et.Z, targetX, targetZ);

        // Reached patrol point — switch direction and go idle briefly
        if (dist < 1.0f)
        {
            ai.MovingToB = !ai.MovingToB;
            ai.State = AIState::Idle;
            ai.StateTimer = 0.0f;
            return;
        }

        // Move toward patrol target
        float dx = targetX - et.X;
        float dz = targetZ - et.Z;
        if (dist > 0.01f)
        {
            float speed = ai.MoveSpeed * 0.5f; // patrol at half speed
            et.X += (dx / dist) * speed * dt;
            et.Z += (dz / dist) * speed * dt;
            et.RotationY = std::atan2(dx, dz);
        }

        if (registry.HasComponent<AnimationState>(enemy))
        {
            auto& anim = registry.GetComponent<AnimationState>(enemy);
            anim.IsMoving = true;
        }
    }

    void EnemyAISystem::UpdateChase(engine::Registry& registry, engine::Entity enemy, engine::Entity player, float dt)
    {
        auto& ai = registry.GetComponent<EnemyAI>(enemy);
        auto& et = registry.GetComponent<Transform>(enemy);
        const auto& pt = registry.GetComponent<Transform>(player);

        float dist = Distance(et.X, et.Z, pt.X, pt.Z);

        if (dist < ai.AttackRange)
        {
            ai.State = AIState::Attack;
            ai.StateTimer = 0.0f;
            return;
        }

        if (ai.StateTimer > ai.MaxChaseTime)
        {
            ai.State = AIState::Idle;
            ai.StateTimer = 0.0f;
            ai.LoseTargetTimer = ai.LoseTargetCooldown;
            return;
        }

        if (dist > ai.VisionRange * 1.5f)
        {
            ai.State = AIState::Idle;
            ai.StateTimer = 0.0f;
            return;
        }

        float dx = pt.X - et.X;
        float dz = pt.Z - et.Z;
        if (dist > 0.01f)
        {
            et.X += (dx / dist) * ai.MoveSpeed * dt;
            et.Z += (dz / dist) * ai.MoveSpeed * dt;
            et.RotationY = std::atan2(dx, dz);
        }

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

        float dx = pt.X - et.X;
        float dz = pt.Z - et.Z;
        if (dist > 0.01f)
            et.RotationY = std::atan2(dx, dz);

        if (registry.HasComponent<AnimationState>(enemy))
        {
            auto& anim = registry.GetComponent<AnimationState>(enemy);
            anim.IsMoving = false;
        }

        ai.AttackTimer += dt;
        if (ai.AttackTimer >= ai.AttackCooldown)
        {
            ai.AttackTimer = 0.0f;

            if (registry.HasComponent<CombatState>(enemy))
            {
                auto& combat = registry.GetComponent<CombatState>(enemy);
                combat.IsAttacking = true;
            }
        }

        if (dist > ai.AttackRange + 1.0f)
        {
            ai.State = AIState::Chase;
            ai.StateTimer = 0.0f;
        }
    }
}

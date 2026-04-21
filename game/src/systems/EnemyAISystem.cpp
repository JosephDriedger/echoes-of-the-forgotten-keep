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

    float EnemyAISystem::DistanceSq(float ax, float az, float bx, float bz)
    {
        float dx = ax - bx;
        float dz = az - bz;
        return dx * dx + dz * dz;
    }

    void EnemyAISystem::Update(engine::Registry& registry, float deltaTime)
    {
        // Cache player entity; only re-search when the cached handle goes stale.
        if (!m_CachedPlayer.IsValid() || !registry.IsAlive(m_CachedPlayer) ||
            !registry.HasComponent<Player>(m_CachedPlayer))
        {
            m_CachedPlayer = engine::Entity(0);
            for (const engine::Entity entity : registry.GetActiveEntities())
            {
                if (registry.HasComponent<Player>(entity) && registry.HasComponent<Transform>(entity))
                {
                    m_CachedPlayer = entity;
                    break;
                }
            }
        }
        engine::Entity playerEntity = m_CachedPlayer;

        // Update each enemy
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<EnemyAI>(entity) ||
                !registry.HasComponent<Transform>(entity))
            {
                continue;
            }

            auto& ai = registry.GetComponent<EnemyAI>(entity);
            auto& et = registry.GetComponent<Transform>(entity);

            // Skip dead enemies (handled by DeathSystem)
            if (registry.HasComponent<Health>(entity) &&
                registry.GetComponent<Health>(entity).Current <= 0)
            {
                continue;
            }

            // Knockback override: linearly decelerate the enemy away from
            // the damage source. Skips all AI state logic until recovery.
            if (ai.IsKnockedBack)
            {
                ai.KnockbackTimer -= deltaTime;

                float t = ai.KnockbackTimer / ai.KnockbackDuration; // 1->0 deceleration
                et.X += ai.KnockbackVX * ai.KnockbackSpeed * t * deltaTime;
                et.Z += ai.KnockbackVZ * ai.KnockbackSpeed * t * deltaTime;

                if (ai.KnockbackTimer <= 0.0f)
                {
                    ai.IsKnockedBack = false;
                    ai.State         = AIState::Chase; // resume chasing after recovery
                    ai.StateTimer    = 0.0f;
                }

                continue; // skip all AI state logic this frame
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
                const auto& pt = registry.GetComponent<Transform>(playerEntity);
                float distSq = DistanceSq(et.X, et.Z, pt.X, pt.Z);
                if (distSq < ai.VisionRange * ai.VisionRange)
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

        // Close enough to attack -- set timer to cooldown so the first
        // attack fires immediately upon entering Attack state.
        if (dist < ai.AttackRange)
        {
            ai.State = AIState::Attack;
            ai.StateTimer = 0.0f;
            ai.AttackTimer = ai.AttackCooldown;
            return;
        }

        // Give up chasing after MaxChaseTime; apply a cooldown before
        // the enemy can re-detect the player (prevents instant re-aggro).
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

        // Hysteresis buffer (+1 unit) prevents flickering between Attack and Chase.
        if (dist > ai.AttackRange + 1.0f)
        {
            ai.State = AIState::Chase;
            ai.StateTimer = 0.0f;
        }
    }
}

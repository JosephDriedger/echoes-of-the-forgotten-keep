// Created by Elijah Fabon

#include "game/systems/AttackHitboxSystem.h"

#include "game/components/Components.h"

#include <cmath>
#include <unordered_set>

namespace game
{
    void AttackHitboxSystem::Update(engine::Registry& registry)
    {
        std::unordered_set<uint32_t> stillAttacking;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<CombatState>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            const auto& combat = registry.GetComponent<CombatState>(entity);
            const auto& transform = registry.GetComponent<Transform>(entity);
            uint32_t id = entity.GetId();

            if (!combat.IsAttacking)
                continue;

            stillAttacking.insert(id);

            if (!registry.HasComponent<AttackStateComponent>(entity))
                registry.AddComponent(entity, AttackStateComponent{});

            auto& state = registry.GetComponent<AttackStateComponent>(entity);

            bool hitboxExists = state.HasActiveHitbox && registry.IsAlive(state.ActiveHitbox);
            bool newSwing = !hitboxExists || state.LastComboIndex != combat.ComboIndex;

            if (newSwing)
            {
                if (hitboxExists)
                {
                    registry.DestroyEntity(state.ActiveHitbox);
                    state.HasActiveHitbox = false;
                }

                engine::Entity hitbox = registry.CreateEntity();
                Collider hitboxCollider(2.0f, 1.5f, 2.0f);
                hitboxCollider.IsTrigger = true;
                hitboxCollider.IsStatic = false;
                registry.AddComponent(hitbox, Transform());
                registry.AddComponent(hitbox, hitboxCollider);

                state.ActiveHitbox = hitbox;
                state.HasActiveHitbox = true;
                state.HitThisSwing.clear();
                state.LastComboIndex = combat.ComboIndex;
            }

            // Position hitbox in front of the attacker
            float offsetDist = 1.0f;
            float hitX = transform.X + std::sin(transform.RotationY) * offsetDist;
            float hitZ = transform.Z + std::cos(transform.RotationY) * offsetDist;

            auto& hitTransform = registry.GetComponent<Transform>(state.ActiveHitbox);
            hitTransform.X = hitX;
            hitTransform.Y = transform.Y + 0.5f;
            hitTransform.Z = hitZ;

            // Check overlaps against valid targets
            for (const engine::Entity target : registry.GetActiveEntities())
            {
                if (target == entity) continue;
                uint32_t targetId = target.GetId();

                if (state.HitThisSwing.count(targetId)) continue;

                if (!registry.HasComponent<CombatState>(target) ||
                    !registry.HasComponent<Health>(target) ||
                    !registry.HasComponent<Transform>(target) ||
                    !registry.HasComponent<Collider>(target))
                    continue;

                // Don't let enemies hit each other
                bool attackerIsEnemy = registry.HasComponent<EnemyAI>(entity);
                bool targetIsEnemy   = registry.HasComponent<EnemyAI>(target);
                if (attackerIsEnemy && targetIsEnemy) continue;

                const auto& hitboxTransform = registry.GetComponent<Transform>(state.ActiveHitbox);
                const auto& hitboxCollider  = registry.GetComponent<Collider>(state.ActiveHitbox);
                const auto& targetTransform = registry.GetComponent<Transform>(target);
                const auto& targetCollider  = registry.GetComponent<Collider>(target);

                // Simple AABB overlap
                bool overlapX = std::abs(hitboxTransform.X - targetTransform.X) < (hitboxCollider.Width  + targetCollider.Width)  * 0.5f;
                bool overlapY = std::abs(hitboxTransform.Y - targetTransform.Y) < (hitboxCollider.Height + targetCollider.Height) * 0.5f;
                bool overlapZ = std::abs(hitboxTransform.Z - targetTransform.Z) < (hitboxCollider.Depth  + targetCollider.Depth)  * 0.5f;

                if (overlapX && overlapY && overlapZ)
                {
                    auto& targetCombat = registry.GetComponent<CombatState>(target);
                    if (!targetCombat.IsDead)
                    {
                        if (targetCombat.HitTimer > 0.0f) continue;

                        // 3rd hit in the combo (index 2) applies extra knockback
                        bool isLastHit = (combat.ComboIndex == 2);
                        float knockbackMult = isLastHit ? combat.FinalHitKnockbackMultiplier : 1.0f;

                        targetCombat.IncomingHit = PendingHit{ combat.AttackDamage, entity, knockbackMult };
                        state.HitThisSwing.insert(targetId);
                    }
                }
            }
        }

        // Clean up hitboxes for entities that stopped attacking
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<AttackStateComponent>(entity)) continue;
            if (stillAttacking.count(entity.GetId())) continue;

            auto& state = registry.GetComponent<AttackStateComponent>(entity);
            if (state.HasActiveHitbox && registry.IsAlive(state.ActiveHitbox))
                registry.DestroyEntity(state.ActiveHitbox);

            state.HasActiveHitbox = false;
            state.HitThisSwing.clear();
        }
    }
}

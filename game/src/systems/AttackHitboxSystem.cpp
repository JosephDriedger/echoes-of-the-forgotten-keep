#include "game/systems/AttackHitboxSystem.h"

#include "game/components/CombatState.h"
#include "game/components/Transform.h"
#include "game/components/Collider.h"
#include "game/components/Player.h"
#include "game/components/EnemyAI.h"
#include "game/components/Health.h"

#include <cmath>

namespace game
{
    void AttackHitboxSystem::Update(engine::Registry& registry)
    {
        // Collect all attacking entities this frame
        std::unordered_set<uint32_t> stillAttacking;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<CombatState>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            const auto& combat = registry.GetComponent<CombatState>(entity);
            const auto& transform = registry.GetComponent<Transform>(entity);
            uint32_t id = entity.GetId();

            if (combat.IsAttacking)
            {
                stillAttacking.insert(id);

                // Spawn hitbox if not already active
                if (m_ActiveHitboxes.find(id) == m_ActiveHitboxes.end() ||
                    !registry.IsAlive(m_ActiveHitboxes[id]))
                {
                    engine::Entity hitbox = registry.CreateEntity();

                    Collider hitboxCollider(2.0f, 1.5f, 2.0f);
                    hitboxCollider.IsTrigger = true;
                    hitboxCollider.IsStatic = false;
                    registry.AddComponent(hitbox, Transform());
                    registry.AddComponent(hitbox, hitboxCollider);

                    m_ActiveHitboxes[id] = hitbox;
                    m_HitThisSwing[id].clear();
                }

                // Position hitbox in front of the attacker
                float offsetDist = 1.0f;
                float hitX = transform.X + std::sin(transform.RotationY) * offsetDist;
                float hitZ = transform.Z + std::cos(transform.RotationY) * offsetDist;

                auto& hitTransform = registry.GetComponent<Transform>(m_ActiveHitboxes[id]);
                hitTransform.X = hitX;
                hitTransform.Y = transform.Y + 0.5f;
                hitTransform.Z = hitZ;

                // Check overlaps against valid targets
                for (const engine::Entity target : registry.GetActiveEntities())
                {
                    if (target == entity) continue;
                    uint32_t targetId = target.GetId();

                    if (m_HitThisSwing[id].count(targetId)) continue;

                    if (!registry.HasComponent<CombatState>(target) ||
                        !registry.HasComponent<Health>(target) ||
                        !registry.HasComponent<Transform>(target) ||
                        !registry.HasComponent<Collider>(target))
                        continue;

                    // Don't let enemies hit each other
                    bool attackerIsEnemy = registry.HasComponent<EnemyAI>(entity);
                    bool targetIsEnemy   = registry.HasComponent<EnemyAI>(target);
                    if (attackerIsEnemy && targetIsEnemy) continue;

                    const auto& hitboxTransform  = registry.GetComponent<Transform>(m_ActiveHitboxes[id]);
                    const auto& hitboxCollider   = registry.GetComponent<Collider>(m_ActiveHitboxes[id]);
                    const auto& targetTransform  = registry.GetComponent<Transform>(target);
                    const auto& targetCollider   = registry.GetComponent<Collider>(target);

                    // Simple AABB overlap
                    bool overlapX = std::abs(hitboxTransform.X - targetTransform.X) < (hitboxCollider.Width  + targetCollider.Width)  * 0.5f;
                    bool overlapY = std::abs(hitboxTransform.Y - targetTransform.Y) < (hitboxCollider.Height + targetCollider.Height) * 0.5f;
                    bool overlapZ = std::abs(hitboxTransform.Z - targetTransform.Z) < (hitboxCollider.Depth  + targetCollider.Depth)  * 0.5f;

                    if (overlapX && overlapY && overlapZ)
                    {
                        auto& targetCombat = registry.GetComponent<CombatState>(target);
                        if (!targetCombat.IsDead)
                        {
                            targetCombat.IncomingHit = PendingHit{ combat.AttackDamage, entity };
                            m_HitThisSwing[id].insert(targetId);
                        }
                    }
                }
            }
        }

        // Clean up hitboxes for entities that stopped attacking
        for (auto it = m_ActiveHitboxes.begin(); it != m_ActiveHitboxes.end(); )
        {
            if (stillAttacking.find(it->first) == stillAttacking.end())
            {
                if (registry.IsAlive(it->second))
                    registry.DestroyEntity(it->second);

                m_HitThisSwing.erase(it->first);
                it = m_ActiveHitboxes.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}
#include "game/systems/AttackHitboxSystem.h"

#include "game/components/Player.h"
#include "game/components/CombatState.h"
#include "game/components/Transform.h"
#include "game/components/Collider.h"

#include <cmath>

namespace game
{
    void AttackHitboxSystem::Update(engine::Registry& registry)
    {
        // Find the player entity
        engine::Entity playerEntity(0);
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (registry.HasComponent<Player>(entity) &&
                registry.HasComponent<CombatState>(entity) &&
                registry.HasComponent<Transform>(entity))
            {
                playerEntity = entity;
                break;
            }
        }

        if (!playerEntity.IsValid() || !registry.IsAlive(playerEntity))
            return;

        const auto& combat = registry.GetComponent<CombatState>(playerEntity);
        const auto& playerTransform = registry.GetComponent<Transform>(playerEntity);

        if (combat.IsAttacking)
        {
            // Spawn hitbox if it doesn't exist
            if (m_AttackHitbox.GetId() == 0 || !registry.IsAlive(m_AttackHitbox))
            {
                m_AttackHitbox = registry.CreateEntity();

                Collider hitboxCollider(2.0f, 1.5f, 2.0f);
                hitboxCollider.IsTrigger = true;
                hitboxCollider.IsStatic = false;
                registry.AddComponent(m_AttackHitbox, Transform());
                registry.AddComponent(m_AttackHitbox, hitboxCollider);
            }

            // Position hitbox in front of the player based on facing direction
            float offsetDist = 1.0f;
            float hitX = playerTransform.X + std::sin(playerTransform.RotationY) * offsetDist;
            float hitZ = playerTransform.Z + std::cos(playerTransform.RotationY) * offsetDist;

            auto& hitTransform = registry.GetComponent<Transform>(m_AttackHitbox);
            hitTransform.X = hitX;
            hitTransform.Y = playerTransform.Y + 0.5f;
            hitTransform.Z = hitZ;
        }
        else
        {
            // Destroy hitbox when not attacking
            if (m_AttackHitbox.GetId() != 0 && registry.IsAlive(m_AttackHitbox))
            {
                registry.DestroyEntity(m_AttackHitbox);
                m_AttackHitbox = engine::Entity(0);
            }
        }
    }
}

//
// Created by scept on 2026-03-11.
//

#include "game/systems/CombatSystem.h"
#include "game/components/Transform.h"
#include "game/components/AttackRequest.h"

namespace game
{
    void CombatSystem::Update(engine::Registry& registry, float dt)
    {
        for (auto entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<AttackRequest>(entity))
                continue;

            auto& req = registry.GetComponent<AttackRequest>(entity);

            SpawnProjectile(registry, entity, req.direction, req.tag);

            registry.RemoveComponent<AttackRequest>(entity);
        }
    }

    void CombatSystem::SpawnProjectile(engine::Registry& registry,
                                       engine::Entity attacker,
                                       const glm::vec3& direction,
                                       const std::string& tag)
    {
        if (glm::length(direction) == 0.0f)
            return;

        if (!registry.HasComponent<Transform>(attacker))
            return;

        auto& attackerTransform = registry.GetComponent<Transform>(attacker);

        engine::Entity proj = registry.CreateEntity();

        Transform t;
        t.X = attackerTransform.X + direction.x * 1.3f;
        t.Y = attackerTransform.Y;
        t.Z = attackerTransform.Z + direction.z * 1.3f;

        registry.AddComponent(proj, t);

        // Add more components later (velocity, collider, damage, etc.)
    }
}
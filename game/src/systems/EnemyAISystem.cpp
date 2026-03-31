//
// Created by scept on 2026-03-09.
//

#include "game/systems/EnemyAISystem.h"
#include <glm/glm.hpp>

namespace game
{
    void EnemyAISystem::Update(engine::Registry& registry, float dt)
    {
        engine::Entity player = FindPlayer(registry);

        for (auto entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<AI>(entity) ||
                !registry.HasComponent<Transform>(entity))
                continue;

            auto& ai = registry.GetComponent<AI>(entity);
            auto& transform = registry.GetComponent<Transform>(entity);

            ai.stateTimer += dt;

            if (!registry.IsAlive(player))
                continue;

            auto& playerTransform = registry.GetComponent<Transform>(player);

            glm::vec3 dir = {
                playerTransform.X - transform.X,
                0,
                playerTransform.Z - transform.Z
            };

            float dist = glm::length(dir);

            switch (ai.state)
            {
                case AIState::Idle:
                {
                    if (dist < ai.visionRange)
                    {
                        ai.state = AIState::Chase;
                        ai.stateTimer = 0;
                    }
                    break;
                }

                case AIState::Chase:
                {
                    if (dist > ai.visionRange * 1.5f)
                    {
                        ai.state = AIState::Idle;
                        break;
                    }

                    glm::vec3 move = Normalize(dir);
                    transform.X += move.x * dt * 2.5f;
                    transform.Z += move.z * dt * 2.5f;

                    if (dist < ai.combat.attackRange)
                    {
                        ai.state = AIState::Attack;
                        ai.stateTimer = 0;
                    }

                    break;
                }

                case AIState::Attack:
                {
                    if (dist > ai.combat.attackRange + 1.0f)
                    {
                        ai.state = AIState::Chase;
                        break;
                    }

                    // Trigger attack (you can expand this)
                    if (ai.stateTimer > ai.attack.cooldown)
                    {
                        ai.stateTimer = 0;
                        registry.AddComponent(entity, AttackRequest{Normalize(dir), "projectile"});
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }

    engine::Entity EnemyAISystem::FindPlayer(engine::Registry& registry)
    {
        for (auto entity : registry.GetActiveEntities())
        {
            if (registry.HasComponent<Player>(entity))
                return entity;
        }
        return engine::Entity(0);
    }

    float EnemyAISystem::Distance(const Transform& a, const Transform& b)
    {
        return glm::distance(
            glm::vec3(a.X, a.Y, a.Z),
            glm::vec3(b.X, b.Y, b.Z)
        );
    }

    glm::vec3 EnemyAISystem::Normalize(const glm::vec3& v)
    {
        float len = glm::length(v);
        return (len > 0.0001f) ? v / len : glm::vec3(0.0f);
    }
}
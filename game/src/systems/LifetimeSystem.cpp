// Created by Elijah Fabon

#include "game/systems/LifetimeSystem.h"

#include "game/components/Components.h"

namespace game
{
    void LifetimeSystem::Update(engine::Registry& registry, float deltaTime)
    {
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Lifetime>(entity)) continue;

            auto& life = registry.GetComponent<Lifetime>(entity);
            life.Duration -= deltaTime;

            if (life.Duration <= 0.0f)
            {
                registry.DestroyEntity(entity);
            }
        }
    }
}

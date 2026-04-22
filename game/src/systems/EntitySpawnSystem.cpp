// Created by Adam Van Woerden

#include "game/systems/EntitySpawnSystem.h"

#include "game/components/Components.h"

namespace game
{
    // Register every game-specific component type with the ECS registry.
    // Must be called once at startup before any entities are created.
    void EntitySpawnSystem::RegisterComponents(engine::Registry& registry)
    {
        registry.RegisterComponent<Transform>();
        registry.RegisterComponent<Collider>();
        registry.RegisterComponent<Player>();
        registry.RegisterComponent<EnemyAI>();
        registry.RegisterComponent<Render>();
        registry.RegisterComponent<AnimationState>();
        registry.RegisterComponent<BoneAttachment>();
        registry.RegisterComponent<CombatState>();
        registry.RegisterComponent<Door>();
        registry.RegisterComponent<Health>();
        registry.RegisterComponent<Switch>();
        registry.RegisterComponent<Lifetime>();
        registry.RegisterComponent<Knockback>();
        registry.RegisterComponent<CameraFollowState>();
        registry.RegisterComponent<AttackStateComponent>();
    }

    void EntitySpawnSystem::Register(engine::Registry& registry)
    {
        RegisterComponents(registry);
    }

    engine::Entity EntitySpawnSystem::SpawnPlayer(engine::Registry& registry)
    {
        const engine::Entity entity = registry.CreateEntity();

        registry.AddComponent(entity, Transform(0.0f, 0.0f, 0.0f));
        registry.AddComponent(entity, Collider(1.0f, 1.0f, 1.0f));
        registry.AddComponent(entity, Player());

        return entity;
    }
}
#include "game/systems/EntitySpawnSystem.h"

#include "game/components/Collider.h"
#include "game/components/CombatState.h"
#include "game/components/EnemyAI.h"
#include "game/components/Player.h"
#include "game/components/Transform.h"
#include "game/components/Render.h"
#include "game/components/AnimationState.h"
#include "game/components/BoneAttachment.h"
#include "game/components/Door.h"
#include "game/components/Health.h"
#include "game/components/Switch.h"

namespace game
{
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

    engine::Entity EntitySpawnSystem::SpawnEnemy(engine::Registry& registry, const float x, const float y)
    {
        const engine::Entity entity = registry.CreateEntity();

        registry.AddComponent(entity, Transform(x, y, 0.0f));
        registry.AddComponent(entity, Collider(1.0f, 1.0f, 1.0f));
        registry.AddComponent(entity, EnemyAI());

        return entity;
    }

    engine::Entity EntitySpawnSystem::SpawnExit(engine::Registry& registry, const float x, const float y)
    {
        const engine::Entity entity = registry.CreateEntity();

        registry.AddComponent(entity, Transform(x, y, 0.0f));
        registry.AddComponent(entity, Collider(1.0f, 1.0f, 1.0f, true));

        return entity;
    }
}
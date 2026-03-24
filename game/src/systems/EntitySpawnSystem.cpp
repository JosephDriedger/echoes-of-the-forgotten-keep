#include "game/systems/EntitySpawnSystem.h"

#include "game/components/Collider.h"
#include "game/components/EnemyAI.h"
#include "game/components/Player.h"
#include "game/components/Transform.h"

namespace game
{
    void EntitySpawnSystem::RegisterComponents(engine::Registry& registry)
    {
        registry.RegisterComponent<Transform>();
        registry.RegisterComponent<Collider>();
        registry.RegisterComponent<Player>();
        registry.RegisterComponent<EnemyAI>();
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
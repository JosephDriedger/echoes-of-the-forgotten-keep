#include "game/systems/EntitySpawnSystem.h"

#include "engine/ecs/Registry.h"
#include "game/components/Collider.h"
#include "game/components/EnemyAI.h"
#include "game/components/Player.h"
#include "game/components/Transform.h"
#include "game/components/Render.h"
#include "game/components/AnimationState.h"
#include "game/components/BoneAttachment.h"

#include <cassert>
#include <iostream>

int RunEntitySpawnSystemTests()
{
    // Test: RegisterComponents registers all expected component types
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        assert(registry.IsComponentRegistered<game::Transform>());
        assert(registry.IsComponentRegistered<game::Collider>());
        assert(registry.IsComponentRegistered<game::Player>());
        assert(registry.IsComponentRegistered<game::EnemyAI>());
        assert(registry.IsComponentRegistered<game::Render>());
        assert(registry.IsComponentRegistered<game::AnimationState>());
        assert(registry.IsComponentRegistered<game::BoneAttachment>());
    }

    // Test: SpawnPlayer creates entity with correct components
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity player = game::EntitySpawnSystem::SpawnPlayer(registry);

        assert(registry.IsAlive(player));
        assert(registry.HasComponent<game::Transform>(player));
        assert(registry.HasComponent<game::Collider>(player));
        assert(registry.HasComponent<game::Player>(player));

        const auto& transform = registry.GetComponent<game::Transform>(player);
        assert(transform.X == 0.0f);
        assert(transform.Y == 0.0f);
        assert(transform.Z == 0.0f);

        const auto& collider = registry.GetComponent<game::Collider>(player);
        assert(collider.Width == 1.0f);
        assert(collider.Height == 1.0f);
        assert(collider.Depth == 1.0f);
        assert(!collider.IsTrigger);
    }

    // Test: SpawnEnemy creates entity with correct components and position
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity enemy = game::EntitySpawnSystem::SpawnEnemy(registry, 5.0f, 7.0f);

        assert(registry.IsAlive(enemy));
        assert(registry.HasComponent<game::Transform>(enemy));
        assert(registry.HasComponent<game::Collider>(enemy));
        assert(registry.HasComponent<game::EnemyAI>(enemy));
        assert(!registry.HasComponent<game::Player>(enemy));

        const auto& transform = registry.GetComponent<game::Transform>(enemy);
        assert(transform.X == 5.0f);
        assert(transform.Y == 7.0f);

        const auto& ai = registry.GetComponent<game::EnemyAI>(enemy);
        assert(ai.MoveSpeed == 1.0f);
    }

    // Test: SpawnExit creates trigger collider
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity exit = game::EntitySpawnSystem::SpawnExit(registry, 3.0f, 4.0f);

        assert(registry.IsAlive(exit));
        assert(registry.HasComponent<game::Transform>(exit));
        assert(registry.HasComponent<game::Collider>(exit));

        const auto& transform = registry.GetComponent<game::Transform>(exit);
        assert(transform.X == 3.0f);
        assert(transform.Y == 4.0f);

        const auto& collider = registry.GetComponent<game::Collider>(exit);
        assert(collider.IsTrigger);
    }

    // Test: Multiple entities can be spawned
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity player = game::EntitySpawnSystem::SpawnPlayer(registry);
        engine::Entity enemy1 = game::EntitySpawnSystem::SpawnEnemy(registry, 1.0f, 1.0f);
        engine::Entity enemy2 = game::EntitySpawnSystem::SpawnEnemy(registry, 2.0f, 2.0f);
        engine::Entity exit = game::EntitySpawnSystem::SpawnExit(registry, 10.0f, 10.0f);

        assert(registry.GetAliveCount() == 4);
        assert(registry.IsAlive(player));
        assert(registry.IsAlive(enemy1));
        assert(registry.IsAlive(enemy2));
        assert(registry.IsAlive(exit));
    }

    std::cout << "Entity spawn system tests passed.\n";
    return 0;
}

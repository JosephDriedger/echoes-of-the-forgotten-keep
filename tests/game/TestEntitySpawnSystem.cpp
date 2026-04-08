/// @file TestEntitySpawnSystem.cpp
/// @brief Tests for EntitySpawnSystem component registration and player spawning.
///
/// Verifies that Register() makes all game component types available in
/// the Registry, that SpawnPlayer creates an entity with the expected
/// components and default values, and that multiple players can coexist.

#include "game/systems/EntitySpawnSystem.h"

#include "engine/ecs/Registry.h"
#include "game/components/Components.h"

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

    // Test: Multiple players can be spawned
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity player1 = game::EntitySpawnSystem::SpawnPlayer(registry);
        engine::Entity player2 = game::EntitySpawnSystem::SpawnPlayer(registry);

        assert(registry.GetAliveCount() == 2);
        assert(registry.IsAlive(player1));
        assert(registry.IsAlive(player2));
    }

    std::cout << "Entity spawn system tests passed.\n";
    return 0;
}

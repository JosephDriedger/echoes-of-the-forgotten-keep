//
// Created by Joseph Driedger on 3/23/2026.
//

#include "game/systems/EntitySpawnSystem.h"

#include <cassert>
#include <iostream>

#include "engine/ecs/Registry.h"
#include "game/components/EnemyAI.h"
#include "game/components/Transform.h"

int RunEnemyAITests()
{
    engine::Registry registry;
    game::EntitySpawnSystem::RegisterComponents(registry);

    const engine::Entity enemy = game::EntitySpawnSystem::SpawnEnemy(registry, 5.0f, 7.0f);

    assert(registry.IsAlive(enemy));
    assert(registry.HasComponent<game::EnemyAI>(enemy));
    assert(registry.HasComponent<game::Transform>(enemy));

    const game::Transform& transform = registry.GetComponent<game::Transform>(enemy);
    const game::EnemyAI& enemyAI = registry.GetComponent<game::EnemyAI>(enemy);

    assert(transform.X == 5.0f);
    assert(transform.Y == 7.0f);
    assert(enemyAI.MoveSpeed == 1.0f);

    std::cout << "Enemy AI tests passed.\n";
    return 0;
}
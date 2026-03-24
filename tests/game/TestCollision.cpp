//
// Created by Joseph Driedger on 3/23/2026.
//

#include "game/systems/CollisionSystem.h"

#include <cassert>
#include <iostream>

#include "engine/ecs/Registry.h"
#include "game/components/Collider.h"
#include "game/components/Transform.h"

int RunCollisionTests()
{
    engine::Registry registry;
    registry.RegisterComponent<game::Transform>();
    registry.RegisterComponent<game::Collider>();

    const engine::Entity entityA = registry.CreateEntity();
    const engine::Entity entityB = registry.CreateEntity();

    registry.AddComponent(entityA, game::Transform(0.0f, 0.0f));
    registry.AddComponent(entityA, game::Collider(2.0f, 2.0f));

    registry.AddComponent(entityB, game::Transform(1.0f, 0.0f));
    registry.AddComponent(entityB, game::Collider(2.0f, 2.0f));

    game::CollisionSystem collisionSystem;
    collisionSystem.Update(registry);

    const game::Transform& transformA = registry.GetComponent<game::Transform>(entityA);
    const game::Transform& transformB = registry.GetComponent<game::Transform>(entityB);

    assert(transformA.X < 0.0f || transformB.X > 1.0f);

    std::cout << "Collision tests passed.\n";
    return 0;
}
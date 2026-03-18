//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/ecs/Registry.h"

#include <cassert>
#include <iostream>

namespace
{
    struct Position
    {
        float X;
        float Y;
    };

    struct Velocity
    {
        float X;
        float Y;

        Velocity(const float x, const float y)
            : X(x),
              Y(y)
        {
        }
    };
}

int main()
{
    engine::Registry registry;

    registry.RegisterComponent<Position>();
    registry.RegisterComponent<Velocity>();

    const engine::Entity entityA = registry.CreateEntity();
    const engine::Entity entityB = registry.CreateEntity();
    const engine::Entity entityC = registry.CreateEntity();

    assert(registry.IsAlive(entityA));
    assert(registry.IsAlive(entityB));
    assert(registry.IsAlive(entityC));
    assert(registry.GetAliveCount() == 3);
    assert(registry.GetActiveEntities().size() == 3);

    registry.AddComponent<Position>(entityA, Position{10.0f, 20.0f});
    registry.AddComponent<Position>(entityB, Position{30.0f, 40.0f});
    registry.EmplaceComponent<Velocity>(entityA, 1.5f, -2.0f);

    assert(registry.HasComponent<Position>(entityA));
    assert(registry.HasComponent<Position>(entityB));
    assert(registry.HasComponent<Velocity>(entityA));
    assert(!registry.HasComponent<Velocity>(entityB));

    const Position& positionA = registry.GetComponent<Position>(entityA);
    assert(positionA.X == 10.0f);
    assert(positionA.Y == 20.0f);

    const engine::Signature& signatureA = registry.GetSignature(entityA);
    assert(signatureA.test(registry.GetComponentType<Position>()));
    assert(signatureA.test(registry.GetComponentType<Velocity>()));

    registry.DestroyEntity(entityB);

    assert(!registry.IsAlive(entityB));
    assert(registry.GetAliveCount() == 2);
    assert(registry.GetActiveEntities().size() == 2);
    assert(!registry.HasComponent<Position>(entityB));

    const engine::Entity reusedEntity = registry.CreateEntity();
    assert(reusedEntity.GetId() == entityB.GetId());
    assert(registry.IsAlive(reusedEntity));
    assert(registry.GetAliveCount() == 3);

    registry.DestroyAllEntities();

    assert(registry.GetAliveCount() == 0);
    assert(registry.GetActiveEntities().empty());
    assert(!registry.IsAlive(entityA));
    assert(!registry.IsAlive(entityC));
    assert(!registry.IsAlive(reusedEntity));

    registry.RegisterComponent<Position>();
    registry.RegisterComponent<Velocity>();

    const engine::Entity resetEntity = registry.CreateEntity();
    registry.AddComponent<Position>(resetEntity, Position{3.0f, 4.0f});

    registry.Reset();

    assert(registry.GetAliveCount() == 0);
    assert(registry.GetRegisteredComponentCount() == 0);
    assert(registry.GetActiveEntities().empty());

    std::cout << "ECS passes Tests.\n";
    return 0;
}
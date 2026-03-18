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

    const engine::Entity entity = registry.CreateEntity();

    registry.AddComponent<Position>(entity, Position{10.0f, 20.0f});
    assert(registry.HasComponent<Position>(entity));

    const Position& position = registry.GetComponent<Position>(entity);
    assert(position.X == 10.0f);
    assert(position.Y == 20.0f);

    Velocity& velocity = registry.EmplaceComponent<Velocity>(entity, 1.5f, -2.0f);
    assert(velocity.X == 1.5f);
    assert(velocity.Y == -2.0f);
    assert(registry.HasComponent<Velocity>(entity));

    const engine::Signature& signatureAfterAdd = registry.GetSignature(entity);
    assert(signatureAfterAdd.test(registry.GetComponentType<Position>()));
    assert(signatureAfterAdd.test(registry.GetComponentType<Velocity>()));

    registry.RemoveComponent<Position>(entity);
    assert(!registry.HasComponent<Position>(entity));
    assert(registry.HasComponent<Velocity>(entity));

    const engine::Signature& signatureAfterRemove = registry.GetSignature(entity);
    assert(!signatureAfterRemove.test(registry.GetComponentType<Position>()));
    assert(signatureAfterRemove.test(registry.GetComponentType<Velocity>()));

    registry.DestroyEntity(entity);
    assert(!registry.IsAlive(entity));
    assert(registry.GetAliveCount() == 0);

    const engine::Entity anotherEntity = registry.CreateEntity();
    registry.AddComponent<Position>(anotherEntity, Position{3.0f, 4.0f});

    registry.Reset();

    assert(registry.GetAliveCount() == 0);
    assert(registry.GetRegisteredComponentCount() == 0);
    assert(registry.GetActiveEntities().empty());

    std::cout << "ECS passes Tests.\n";
    return 0;
}
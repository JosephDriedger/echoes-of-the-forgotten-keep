//
// Created by Joseph Driedger on 3/23/2026.
//

#include "game/systems/RoomTransitionSystem.h"

#include <cassert>
#include <iostream>

#include "engine/ecs/Registry.h"
#include "engine/scene/RoomManager.h"
#include "game/components/Components.h"

int RunRoomTransitionTests()
{
    engine::Registry registry;
    registry.RegisterComponent<game::Transform>();
    registry.RegisterComponent<game::Player>();

    engine::RoomManager roomManager;
    roomManager.AddRoom(0);
    roomManager.AddRoom(1);
    roomManager.SetCurrentRoom(0);
    roomManager.SetTransitionTarget(1);

    const engine::Entity player = registry.CreateEntity();
    registry.AddComponent(player, game::Transform(8.0f, 4.0f));
    registry.AddComponent(player, game::Player());

    game::RoomTransitionSystem roomTransitionSystem;
    roomTransitionSystem.Update(registry, roomManager);

    const game::Transform& transform = registry.GetComponent<game::Transform>(player);

    assert(roomManager.GetCurrentRoom() == 1);
    assert(!roomManager.HasTransitionTarget());
    assert(transform.X == 0.0f);
    assert(transform.Y == 0.0f);

    std::cout << "Room transition tests passed.\n";
    return 0;
}
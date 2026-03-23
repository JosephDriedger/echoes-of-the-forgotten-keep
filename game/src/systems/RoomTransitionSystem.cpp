#include "game/systems/RoomTransitionSystem.h"

#include "game/components/Player.h"
#include "game/components/Transform.h"
#include "game/components/Collider.h"

namespace game
{
    void RoomTransitionSystem::Update(engine::Registry& registry,
                                      engine::RoomManager& roomManager) const
    {
        if (!roomManager.HasTransitionTarget())
            return;

        const auto& entities = registry.GetActiveEntities();

        for (auto entity : entities)
        {
            if (!registry.HasComponent<Player>(entity))
                continue;

            auto& transform = registry.GetComponent<Transform>(entity);

            roomManager.SetCurrentRoom(roomManager.GetTransitionTarget());
            roomManager.ClearTransitionTarget();

            transform.X = 0.0f;
            transform.Y = 0.0f;
            transform.Z = 0.0f;
            return;
        }
    }
}
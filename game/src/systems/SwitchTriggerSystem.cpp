#include "game/systems/SwitchTriggerSystem.h"

#include "game/components/Switch.h"
#include "game/components/Collider.h"
#include "game/components/Transform.h"
#include "game/components/Player.h"

#include <cmath>
#include <iostream>

namespace game
{
    void SwitchTriggerSystem::Update(engine::Registry& registry)
    {
        // Find the player
        engine::Entity playerEntity(0);
        float playerX = 0.0f, playerZ = 0.0f;
        float playerW = 0.0f, playerD = 0.0f;

        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (registry.HasComponent<Player>(entity) &&
                registry.HasComponent<Transform>(entity))
            {
                playerEntity = entity;
                const auto& t = registry.GetComponent<Transform>(entity);
                playerX = t.X;
                playerZ = t.Z;

                if (registry.HasComponent<Collider>(entity))
                {
                    const auto& c = registry.GetComponent<Collider>(entity);
                    playerW = c.Width;
                    playerD = c.Depth;
                }
                break;
            }
        }

        if (!playerEntity.IsValid()) return;

        // Check each switch for overlap with player
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Switch>(entity) ||
                !registry.HasComponent<Transform>(entity) ||
                !registry.HasComponent<Collider>(entity))
            {
                continue;
            }

            auto& sw = registry.GetComponent<Switch>(entity);
            const auto& swT = registry.GetComponent<Transform>(entity);
            const auto& swC = registry.GetComponent<Collider>(entity);

            // Simple AABB overlap check
            float swCenterX = swT.X + swC.OffsetX;
            float swCenterZ = swT.Z + swC.OffsetZ;

            bool overlapping =
                std::abs(playerX - swCenterX) < (playerW + swC.Width) * 0.5f &&
                std::abs(playerZ - swCenterZ) < (playerD + swC.Depth) * 0.5f;

            if (overlapping && !sw.Pressed)
            {
                sw.Pressed = true;
                std::cout << "[SwitchTrigger] Button pressed: " << sw.Id << "\n";
            }
            else if (!overlapping && sw.Pressed)
            {
                sw.Pressed = false;
                std::cout << "[SwitchTrigger] Button released: " << sw.Id << "\n";
            }
        }
    }
}

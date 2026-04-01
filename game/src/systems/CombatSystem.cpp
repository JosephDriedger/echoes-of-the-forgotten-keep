#include "game/systems/CombatSystem.h"

#include "game/components/CombatState.h"
#include "game/components/Transform.h"
#include "game/components/Player.h"
#include "game/components/EnemyAI.h"

namespace game
{
    void CombatSystem::Update(engine::Registry& registry, float deltaTime)
    {
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<CombatState>(entity))
                continue;

            auto& combat = registry.GetComponent<CombatState>(entity);

            // Tick hit timer (used by animation system for hit-react duration)
            if (combat.HitTimer > 0.0f)
                combat.HitTimer -= deltaTime;
        }
    }
}
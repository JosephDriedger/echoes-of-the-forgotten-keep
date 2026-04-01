#include "game/systems/DamageSystem.h"

#include "game/components/Health.h"
#include "game/components/CombatState.h"

namespace game
{
    void DamageSystem::Update(engine::Registry& registry)
    {
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<Health>(entity) ||
                !registry.HasComponent<CombatState>(entity))
                continue;

            auto& health = registry.GetComponent<Health>(entity);
            auto& combat = registry.GetComponent<CombatState>(entity);

            if (combat.IncomingHit.has_value())
            {
                health.Current -= combat.IncomingHit->Damage;
                combat.IncomingHit.reset();

                if (health.Current <= 0)
                    combat.IsDead = true;
            }
        }
    }
}
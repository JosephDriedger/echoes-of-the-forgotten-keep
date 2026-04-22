// Created by Elijah Fabon

#include "game/systems/DamageSystem.h"
#include <cmath>
#include "game/components/Components.h"

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

                // Enemy-specific: compute knockback direction and interrupt AI
                if (registry.HasComponent<Knockback>(entity) &&
                    registry.HasComponent<Transform>(entity))
                {
                    engine::Entity source = combat.IncomingHit->Source;

                    if (registry.IsAlive(source) && registry.HasComponent<Transform>(source))
                    {
                        // Knockback direction: normalized vector from attacker to victim
                        const auto& selfT   = registry.GetComponent<Transform>(entity);
                        const auto& sourceT = registry.GetComponent<Transform>(source);

                        float dx = selfT.X - sourceT.X;
                        float dz = selfT.Z - sourceT.Z;
                        float len = std::sqrt(dx * dx + dz * dz);
                        if (len > 0.01f) { dx /= len; dz /= len; }

                        float knockbackMult = combat.IncomingHit->KnockbackMultiplier;

                        auto& kb = registry.GetComponent<Knockback>(entity);
                        kb.IsKnockedBack    = true;
                        kb.KnockbackTimer   = kb.KnockbackDuration;
                        kb.KnockbackVX      = dx;
                        kb.KnockbackVZ      = dz;
                        kb.KnockbackSpeed   = kb.KnockbackSpeed * knockbackMult;

                    }

                    if (registry.HasComponent<EnemyAI>(entity))
                    {
                        // Force AI back to Idle so it re-evaluates after recovering
                        auto& ai      = registry.GetComponent<EnemyAI>(entity);
                        ai.State      = AIState::Idle;
                        ai.StateTimer = 0.0f;
                    }

                    // Trigger hit animation
                    if (registry.HasComponent<AnimationState>(entity))
                    {
                        auto& anim = registry.GetComponent<AnimationState>(entity);
                        if (anim.HitClipIndex >= 0)
                        {
                            anim.CurrentClip  = anim.HitClipIndex;
                            anim.CurrentTime  = 0.0f;
                            anim.CurrentState = AnimState::HitReact;
                        }
                    }
                }

                combat.HitTimer = 0.5f; // i-frame window (seconds); ticked down by HitTimerSystem
                combat.IncomingHit.reset();

                if (health.Current <= 0)
                    combat.IsDead = true;
            }
        }
    }
}
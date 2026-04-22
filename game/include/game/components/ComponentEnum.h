//
// Created by adamd on 2026-04-07.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTENUM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTENUM_H

namespace game {
    // -----------------------------------------------------------------------
    // AnimationState -- skeletal animation playback and blending.
    // -----------------------------------------------------------------------
    enum class AnimState
    {
        Idle,
        Run,
        Attack1,
        Attack2,
        Attack3,
        HitReact,
        Death,
        Dash
    };

    // -----------------------------------------------------------------------
    // EnemyAI -- state machine for enemy behavior.
    // -----------------------------------------------------------------------
    enum class AIState { Idle, Patrol, Chase, Attack };

    // -----------------------------------------------------------------------
    // CombatState -- attack state, combo system, and incoming damage.
    // -----------------------------------------------------------------------
    struct PendingHit
    {
        int Damage = 0;
        engine::Entity Source;
        float KnockbackMultiplier = 1.0f;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTENUM_H
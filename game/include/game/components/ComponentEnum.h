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
        Death
    };

    // -----------------------------------------------------------------------
    // EnemyAI -- state machine for enemy behavior.
    // -----------------------------------------------------------------------
    enum class AIState { Idle, Patrol, Chase, Attack };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTENUM_H
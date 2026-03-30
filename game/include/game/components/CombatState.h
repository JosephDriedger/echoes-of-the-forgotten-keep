#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COMBATSTATE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COMBATSTATE_H

namespace game
{
    struct CombatState
    {
        bool IsAttacking = false;
        bool AttackQueued = false;
        bool ComboWindowOpen = false;
        int ComboIndex = 0;
        float ComboWindow = 0.5f;
        float ComboTimer = 0.0f;

        bool IsHit = false;
        bool IsDead = false;
        float HitTimer = 0.0f;

        CombatState() = default;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_COMBATSTATE_H

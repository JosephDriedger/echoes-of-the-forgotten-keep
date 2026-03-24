//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAI_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAI_H

namespace game
{
    struct EnemyAI
    {
        float MoveSpeed;

        explicit EnemyAI(const float moveSpeed = 1.0f)
            : MoveSpeed(moveSpeed)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAI_H
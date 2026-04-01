#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAI_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAI_H

namespace game
{
    enum class AIState { Idle, Patrol, Chase, Attack };

    struct EnemyAI
    {
        float MoveSpeed = 1.0f;

        AIState State = AIState::Idle;
        float StateTimer = 0.0f;

        float VisionRange = 10.0f;
        float AttackRange = 2.0f;
        float AttackCooldown = 2.0f;
        float AttackTimer = 0.0f;

        float MaxChaseTime = 5.0f;
        float LoseTargetCooldown = 2.0f;
        float LoseTargetTimer = 0.0f;

        // Patrol waypoints
        bool HasPatrol = false;
        float PatrolAX = 0.0f, PatrolAZ = 0.0f;
        float PatrolBX = 0.0f, PatrolBZ = 0.0f;
        bool MovingToB = true;
        float IdleBeforePatrol = 2.0f;

        explicit EnemyAI(const float moveSpeed = 1.0f)
            : MoveSpeed(moveSpeed)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAI_H

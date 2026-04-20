// Created by Elijah Fabon

// EnemyAISystem -- Finite state machine controlling enemy behavior.
//
// States and transitions:
//   Idle    -- Standing still. Transitions to Patrol after a timer, or
//              to Chase if the player enters VisionRange.
//   Patrol  -- Moves between two patrol waypoints at half speed.
//              Transitions to Chase on player detection, or back to
//              Idle upon reaching a waypoint.
//   Chase   -- Pursues the player at full MoveSpeed. Transitions to
//              Attack when within AttackRange, back to Idle if the
//              chase exceeds MaxChaseTime or the player moves beyond
//              1.5x VisionRange.
//   Attack  -- Faces the player and attacks on a cooldown timer.
//              Transitions back to Chase if the player moves out of
//              AttackRange + 1 hysteresis buffer.
//
// Knockback interrupts all states: the enemy slides backward with
// decelerating velocity, then resumes chasing.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAISYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAISYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    class EnemyAISystem
    {
    public:
        void Update(engine::Registry& registry, float deltaTime);

    private:
        void UpdateIdle(engine::Registry& registry, engine::Entity enemy, float dt);
        void UpdatePatrol(engine::Registry& registry, engine::Entity enemy, float dt);
        void UpdateChase(engine::Registry& registry, engine::Entity enemy, engine::Entity player, float dt);
        void UpdateAttack(engine::Registry& registry, engine::Entity enemy, engine::Entity player, float dt);

        static float Distance(float ax, float az, float bx, float bz);
        static float DistanceSq(float ax, float az, float bx, float bz);

        engine::Entity m_CachedPlayer;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAISYSTEM_H

// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DEATHSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DEATHSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    // Handles dead entity cleanup: adds Lifetime for corpse removal
    // and sets AnimationState to Death. Runs before EnemyAISystem so
    // dead entities are skipped by AI logic.
    class DeathSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif

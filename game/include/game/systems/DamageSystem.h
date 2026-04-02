// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGESYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class DamageSystem
    {
    public:
        void Update(engine::Registry& registry);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGESYSTEM_H

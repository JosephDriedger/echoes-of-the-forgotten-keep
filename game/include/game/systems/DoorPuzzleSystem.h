// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class DoorPuzzleSystem
    {
    public:
        static void Update(engine::Registry& registry, float deltaTime);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H

// Created by Elijah Fabon

// DoorPuzzleSystem -- Controls trigger-based (puzzle) doors.
// Reads Switch component states and opens/closes doors whose TriggerId
// matches an active switch. Works alongside DoorSystem, which handles
// proximity-based doors separately.

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
